#!/usr/bin/env python3
"""
neuTube unified builder (clang64 on Windows via MSYS2, gcc/clang on Linux)

- On Windows/MSYS2 we use the clang64 environment and *prebuilt static* Qt5 + prebuilt FFTW3 from pacman.
- We STILL build our other C deps from source: zlib-ng(compat), libtiff, libxml2, jansson, pcre2(+posix).
- Then we build neuTube core (C) and the Qt GUI (static).

Quick start (Windows, in MSYS2 CLANG64 shell):
  pacman -Syu
  pacman -S --needed mingw-w64-clang-x86_64-toolchain mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-pkg-config \
                         mingw-w64-clang-x86_64-ninja mingw-w64-clang-x86_64-perl \
                         mingw-w64-clang-x86_64-qt5-static mingw-w64-clang-x86_64-qt5-base \
                         mingw-w64-clang-x86_64-qt5-tools  mingw-w64-clang-x86_64-qt5-imageformats \
                         mingw-w64-clang-x86_64-qt5-svg mingw-w64-clang-x86_64-fftw
  python build.py

Authoring notes:
- Centralized, descriptive names.
- Single place for optimization/linker flags.
- Avoid duplicated helpers; prefer versatile helpers used in multiple steps.
"""

from __future__ import annotations
import argparse, os, platform, shutil, subprocess, sys, tarfile, zipfile, re
from pathlib import Path
from urllib.request import urlopen

IS_WINDOWS = platform.system().lower().startswith("win")
REPO_ROOT  = Path(__file__).resolve().parent

# ---------- Global optimization flags (applied to CMake and qmake builds) ----------
GLOBAL_OPT_CFLAGS  = "-O3 -pipe -fPIC -flto -march=native"
GLOBAL_OPT_CXXFLAGS= GLOBAL_OPT_CFLAGS
GLOBAL_LDFLAGS     = "-flto -static-libgcc -static-libstdc++"

# ---------- Third-party versions we still build ourselves ----------
VERSIONS = {
    "zlibng":  "2.2.4",
    "libtiff": "4.7.1",
    "libxml2": "2.12.7",
    "jansson": "2.14",
    "pcre2":   "10.43",
}
URLS = {
    "zlibng":  lambda v: f"https://github.com/zlib-ng/zlib-ng/archive/refs/tags/{v}.tar.gz",
    "libtiff": lambda v: f"https://download.osgeo.org/libtiff/tiff-{v}.tar.gz",
    "libxml2": lambda v: f"https://gitlab.gnome.org/GNOME/libxml2/-/archive/v{v}/libxml2-v{v}.tar.gz",
    "jansson": lambda v: f"https://github.com/akheron/jansson/archive/refs/tags/v{v}.tar.gz",
    "pcre2":   lambda v: f"https://github.com/PCRE2Project/pcre2/releases/download/pcre2-{v}/pcre2-{v}.tar.gz",
}

# ---------- Small, shared helpers ----------
def ensure_directory(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)

def find_executable_on_path(exe_name: str) -> str | None:
    return shutil.which(exe_name)

def run_checked(command, cwd: Path | None = None, env: dict | None = None, shell: bool = False) -> None:
    printable = command if isinstance(command, str) else " ".join(command)
    print(f"\n>>> {printable}")
    completed = subprocess.run(command, cwd=str(cwd) if cwd else None, env=env, shell=shell)
    if completed.returncode != 0:
        raise RuntimeError(f"Command failed ({completed.returncode}): {printable}")

def download_and_extract(url: str, destination_dir: Path) -> Path:
    ensure_directory(destination_dir)
    archive_path = destination_dir / url.split("/")[-1]
    if not archive_path.exists():
        print(f"downloading: {url}")
        with urlopen(url) as src, open(archive_path, "wb") as dst:
            shutil.copyfileobj(src, dst)
    name = archive_path.name
    if name.endswith((".tar.gz", ".tgz", ".tar.xz", ".tar.bz2")):
        with tarfile.open(archive_path, "r:*") as tf:
            members = tf.getmembers()
            tf.extractall(destination_dir)
        roots = {m.name.split("/", 1)[0] for m in members if "/" in m.name}
        return destination_dir / sorted(roots)[0]
    if name.endswith(".zip"):
        with zipfile.ZipFile(archive_path, "r") as zf:
            zf.extractall(destination_dir)
            names = zf.namelist()
        return destination_dir / names[0].split("/", 1)[0]
    raise ValueError(f"Unknown archive type: {archive_path}")

def prefix_already_has(install_prefix: Path, relative_markers: list[Path]) -> bool:
    return all((install_prefix / rel).exists() for rel in relative_markers)

def jobs_concurrency() -> int:
    return max(1, min((os.cpu_count() or 8), 16))

def prefer_clang64_in_path(base_env: dict) -> dict:
    """
    On Windows, ensure CLANG64 MinGW bin dir is first; keep /usr/bin later.
    This also helps pick Qt/FFTW from clang64 automatically.
    """
    if not IS_WINDOWS:
        return base_env
    env = base_env.copy()
    path_entries = env.get("PATH", "").split(os.pathsep)

    canonical = []
    seen = set()
    for p in path_entries:
        if not p:
            continue
        norm = os.path.normcase(p).replace("/", "\\")
        # filter out Strawberry/old LLVM paths if any
        if "\\strawberry\\" in norm:
            continue
        if norm not in seen:
            seen.add(norm)
            canonical.append(p)

    preferred_bins = [
        r"C:\msys64\clang64\bin",
        r"C:\msys64\usr\bin",  # keep after clang64
    ]
    for p in reversed(preferred_bins):
        if os.path.isdir(p) and p not in canonical:
            canonical.insert(0, p)
    env["PATH"] = os.pathsep.join(canonical)

    # Helpful discovery hints for CMake/pkg-config
    env.setdefault("MSYSTEM", "CLANG64")
    env.setdefault("PKG_CONFIG_PATH", r"C:\msys64\clang64\lib\pkgconfig")
    env.setdefault("CMAKE_PREFIX_PATH", r"C:\msys64\clang64")
    return env

def cmake_common_args(install_prefix: Path, build_type: str, use_clang_first: bool) -> list[str]:
    generator = ["-G", "Ninja"] if find_executable_on_path("ninja") else []
    if use_clang_first:
        c_compiler  = find_executable_on_path("clang")  or "clang"
        cxx_compiler= find_executable_on_path("clang++") or "clang++"
    else:
        c_compiler  = find_executable_on_path("gcc")  or "gcc"
        cxx_compiler= find_executable_on_path("g++")  or "g++"
    return generator + [
        f"-DCMAKE_C_COMPILER={c_compiler}",
        f"-DCMAKE_CXX_COMPILER={cxx_compiler}",
        f"-DCMAKE_BUILD_TYPE={build_type}",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON",
        "-DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON",
        "-DCMAKE_POLICY_VERSION_MINIMUM=3.5",
        f"-DCMAKE_C_FLAGS_RELEASE={GLOBAL_OPT_CFLAGS}",
        f"-DCMAKE_CXX_FLAGS_RELEASE={GLOBAL_OPT_CXXFLAGS}",
        f"-DCMAKE_EXE_LINKER_FLAGS_RELEASE={GLOBAL_LDFLAGS}",
        f"-DCMAKE_SHARED_LINKER_FLAGS_RELEASE={GLOBAL_LDFLAGS}",
        f"-DCMAKE_MODULE_LINKER_FLAGS_RELEASE={GLOBAL_LDFLAGS}",
        f"-DCMAKE_INSTALL_PREFIX={str(install_prefix)}",
    ]

def cmake_configure_build_install(source_dir: Path, build_dir: Path, args: list[str], env: dict, jobs: int) -> None:
    ensure_directory(build_dir)
    run_checked(["cmake", str(source_dir)] + args, cwd=build_dir, env=env)
    run_checked(["cmake", "--build", ".", "--", f"-j{jobs}"], cwd=build_dir, env=env)
    run_checked(["cmake", "--install", "."], cwd=build_dir, env=env)

# ---------- Build third-party deps we still want from source ----------
def build_static_dependencies(args, env):
    install_prefix   = Path(args.install_prefix).resolve()
    work_sources_dir = Path(args.work_dir).resolve()
    build_root_dir   = Path(args.build_dir).resolve()
    ensure_directory(install_prefix); ensure_directory(work_sources_dir); ensure_directory(build_root_dir)

    jobs = jobs_concurrency()
    base = cmake_common_args(install_prefix, "Release", use_clang_first=True)

    # zlib-ng (compat mode)
    z_src = download_and_extract(URLS["zlibng"](VERSIONS["zlibng"]), work_sources_dir)
    z_bld = build_root_dir / "zlibng"
    if not prefix_already_has(install_prefix, [Path("lib") / "libz.a"]) or args.force_deps:
        z_args = base + [
            "-DZLIB_COMPAT=ON",
            "-DZLIB_ENABLE_TESTS=OFF",
            "-DZLIBNG_ENABLE_TESTS=OFF",
            "-DWITH_GTEST=OFF",
            "-DWITH_BENCHMARKS=OFF",
            "-DWITH_BENCHMARK_APPS=OFF",
            "-DWITH_NATIVE_INSTRUCTIONS=ON",
            "-DWITH_NEW_STRATEGIES=ON",
            "-DWITH_OPTIM=ON",
            "-DWITH_AVX2=ON",
            "-DWITH_SANITIZER=OFF",
        ]
        cmake_configure_build_install(z_src, z_bld, z_args, env, jobs)
    else:
        print("[deps] zlib-ng present, skipping.")

    zlib_lib = (install_prefix / "lib" / "libz.a").resolve()
    zlib_inc = (install_prefix / "include").resolve()

    # libtiff (static; keep codecs minimal)
    t_src = download_and_extract(URLS["libtiff"](VERSIONS["libtiff"]), work_sources_dir)
    t_bld = build_root_dir / "libtiff"
    if not prefix_already_has(install_prefix, [Path("lib") / "libtiff.a"]) or args.force_deps:
        t_args = base + [
            "-Dtiff-tools=ON",
            "-Dtiff-tests=OFF",
            "-Dtiff-opengl=OFF",
            "-Dtiff-contrib=OFF",
            "-Dtiff-docs=OFF",
            "-Djbig=OFF","-Djpeg=OFF","-Dold-jpeg=OFF",
            "-Dlzma=OFF","-Dwebp=OFF","-Dlerc=OFF",
            "-Dpixarlog=OFF","-Dzstd=OFF","-Dlibdeflate=OFF",
            "-DZLIB_USE_STATIC_LIBS=ON",
            f"-DZLIB_LIBRARY={zlib_lib.as_posix()}",
            f"-DZLIB_INCLUDE_DIR={zlib_inc.as_posix()}",
        ]
        cmake_configure_build_install(t_src, t_bld, t_args, env, jobs)
    else:
        print("[deps] libtiff present, skipping.")

    # libxml2 (zlib only)
    x_src = download_and_extract(URLS["libxml2"](VERSIONS["libxml2"]), work_sources_dir)
    x_bld = build_root_dir / "libxml2"
    if not prefix_already_has(install_prefix, [Path("lib") / "libxml2.a"]) or args.force_deps:
        x_args = base + [
            "-DLIBXML2_WITH_PYTHON=OFF",
            "-DLIBXML2_WITH_ICU=OFF",
            "-DLIBXML2_WITH_LZMA=OFF",
            "-DLIBXML2_WITH_ZLIB=ON",
            "-DZLIB_USE_STATIC_LIBS=ON",
            f"-DZLIB_LIBRARY={zlib_lib.as_posix()}",
            f"-DZLIB_INCLUDE_DIR={zlib_inc.as_posix()}",
        ]
        cmake_configure_build_install(x_src, x_bld, x_args, env, jobs)
    else:
        print("[deps] libxml2 present, skipping.")

    # jansson (static)
    j_src = download_and_extract(URLS["jansson"](VERSIONS["jansson"]), work_sources_dir)
    j_bld = build_root_dir / "jansson"
    if not prefix_already_has(install_prefix, [Path("lib") / "libjansson.a"]) or args.force_deps:
        j_args = base + [
            "-DJANSSON_BUILD_DOCS=OFF",
            "-DJANSSON_EXAMPLES=OFF",
            "-DJANSSON_BUILD_SHARED_LIBS=OFF",
        ]
        cmake_configure_build_install(j_src, j_bld, j_args, env, jobs)
    else:
        print("[deps] jansson present, skipping.")

    # PCRE2 (+ POSIX wrapper)
    p_src = download_and_extract(URLS["pcre2"](VERSIONS["pcre2"]), work_sources_dir)
    p_bld = build_root_dir / "pcre2"
    if not prefix_already_has(install_prefix, [Path("lib") / "libpcre2-8.a", Path("lib") / "libpcre2-posix.a"]) or args.force_deps:
        p_args = base + [
            "-DPCRE2_BUILD_PCRE2_8=ON",
            "-DPCRE2_BUILD_PCRE2_16=OFF",
            "-DPCRE2_BUILD_PCRE2_32=OFF",
            "-DPCRE2_SUPPORT_UNICODE=ON",
            "-DPCRE2_SUPPORT_JIT=ON",
            "-DPCRE2_BUILD_TESTS=OFF",
            "-DPCRE2_BUILD_PCRE2GREP=OFF",
            "-DPCRE2_BUILD_PCRE2_8T=OFF",
            "-DPCRE2_BUILD_PCRE2POSIX=ON",
            "-DINSTALL_MSVC_PDB=OFF",
        ]
        cmake_configure_build_install(p_src, p_bld, p_args, env, jobs)
    else:
        print("[deps] pcre2 (+posix) present, skipping.")

    print(f"\n[deps] Installed static deps into: {install_prefix}")

# ---------- neuTube core C (src/c) ----------
def build_neutube_core_c(args, env):
    core_source_dir = (REPO_ROOT / "src" / "c").resolve()
    if not core_source_dir.exists():
        print("[core-c] src/c not found; skipping.")
        return

    core_build_dir  = (Path(args.build_dir) / "core_c").resolve()
    install_prefix  = Path(args.install_prefix).resolve()
    ensure_directory(core_build_dir)

    marker_archive = install_prefix / "lib" / "libneurolabi.a"
    if marker_archive.exists() and not args.clean:
        print(f"[core-c] Already built → {marker_archive.parent} (use --clean to rebuild).")
        return

    perl_exe = find_executable_on_path("perl")
    if not perl_exe:
        raise SystemExit("[core-c] Perl is required for template generation (install mingw-w64-clang-x86_64-perl).")

    myers_dir = (REPO_ROOT / "src" / "lib" / "genelib" / "src").resolve()
    base_args = cmake_common_args(install_prefix, "Release", use_clang_first=True)
    cmake_args = base_args + [
        f"-DDEPS_PREFIX={str(install_prefix)}",
        f"-DMYERS_DIR={str(myers_dir)}",
        "-DUSE_LIBFFTW=ON",          # now comes from MSYS2 FFTW
        f"-DUSE_LIBFFTWF={'ON' if args.fftw_single else 'OFF'}",
        "-DUSE_LIBZ=ON",
        "-DUSE_LIBXML2=ON",
        "-DUSE_LIBJANSSON=ON",
        f"-DPERL_EXECUTABLE={perl_exe}",
    ]

    # Make CMake find MSYS2 clang64 prefixes more easily (FFTW, etc.)
    clang64_prefix = Path("C:/msys64/clang64") if IS_WINDOWS else Path("/usr")
    env = env.copy()
    env["CMAKE_PREFIX_PATH"] = os.pathsep.join(filter(None, [
        env.get("CMAKE_PREFIX_PATH", ""),
        str(clang64_prefix),
        str(install_prefix),
    ]))
    env["PKG_CONFIG_PATH"] = os.pathsep.join(filter(None, [
        env.get("PKG_CONFIG_PATH", ""),
        str(clang64_prefix / "lib" / "pkgconfig"),
        str(install_prefix / "lib" / "pkgconfig"),
    ]))

    cmake_configure_build_install(core_source_dir, core_build_dir, cmake_args, env, jobs_concurrency())

    if marker_archive.exists():
        print(f"[core-c] OK → {marker_archive}")
    else:
        print(f"[core-c] Built, but archive not found in {install_prefix / 'lib'}; check build logs.")

# ---------- GUI (qmake using system static Qt 5 from MSYS2) ----------
def build_neutube_gui(args, env):
    """
    Build the Qt GUI as a mostly-static binary with LTO.

    Improved robustness:
      - Early validation of qmake, lib files and headers (in _install and MSYS2 clang64).
      - Prefer static .a archives; fail early if a required static archive cannot be found.
      - Clear diagnostic messages and suggestions for pacman packages on Windows.
      - Reuses existing helpers (run_checked, find_executable_on_path, jobs_concurrency).
    """
    gui_dir = REPO_ROOT / "src" / "gui"
    if not gui_dir.exists():
        print("[gui] src/gui not found; skipping.")
        return

    # --- Toolchain / qmake -----------------------------------------------------
    if IS_WINDOWS:
        msys_root = Path(r"C:\msys64")
        clang64 = msys_root / "clang64"
        qmake_path = clang64 / "bin" / "qmake.exe"
        lib_search_extra = [clang64 / "lib"]
        pkgconfig_search = [clang64 / "lib" / "pkgconfig"]
        # prefer mingw32-make in MSYS2 clang64
        candidate_makes = ["mingw32-make.exe", "mingw32-make", "make.exe", "make"]
        lld_path = clang64 / "bin" / "ld.lld.exe"
    else:
        qmake_path = Path("/usr/bin/qmake")
        lib_search_extra = [Path("/usr/lib"), Path("/usr/local/lib")]
        pkgconfig_search = [Path("/usr/lib/pkgconfig"), Path("/usr/local/lib/pkgconfig")]
        candidate_makes = ["make", "ninja", "gmake"]
        lld_path = Path("/usr/bin/ld.lld")

    # Allow environment override for NEUROLABI_DIR (or fall back to args.install_prefix)
    deps_prefix = Path(args.install_prefix) if getattr(args, "install_prefix", None) else Path(REPO_ROOT / "_install")
    deps_prefix = deps_prefix.resolve()
    deps_include = (deps_prefix / "include").resolve()
    deps_lib = (deps_prefix / "lib").resolve()

    # qmake discovery and sanity
    if not qmake_path.exists():
        # check PATH for qmake fallback
        alt_qmake = find_executable_on_path("qmake") or find_executable_on_path("qmake-qt5") or find_executable_on_path("qmake5")
        if alt_qmake:
            qmake_path = Path(alt_qmake)
        else:
            raise SystemExit(f"[gui] qmake not found at expected location: {qmake_path}. "
                             "Install a static Qt (e.g., qt5-static for MSYS2 clang64) or set QMAKE in PATH.")

    print(f"[gui] Using qmake: {qmake_path}")

    # --- Basic output dirs -----------------------------------------------------
    app_build_dir = REPO_ROOT / ("_app_build_release" if args.config == "release" else "_app_build_debug")
    ensure_directory(app_build_dir)
    dist_dir = REPO_ROOT / "dist"
    ensure_directory(dist_dir)

    # --- Create plugin stub if missing ----------------------------------------
    plugin_stub = gui_dir / "qt_static_plugins.cpp"
    if not plugin_stub.exists():
        plugin_stub.write_text(
            "#ifdef QT_STATIC\n"
            "#include <QtPlugin>\n"
            "#if defined(Q_OS_WIN)\n"
            "Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)\n"
            "#elif defined(Q_OS_LINUX)\n"
            "Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)\n"
            "#elif defined(Q_OS_MAC)\n"
            "Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)\n"
            "#endif\n"
            "Q_IMPORT_PLUGIN(QGifPlugin)\n"
            "Q_IMPORT_PLUGIN(QJpegPlugin)\n"
            "Q_IMPORT_PLUGIN(QTiffPlugin)\n"
            "Q_IMPORT_PLUGIN(QICOPlugin)\n"
            "Q_IMPORT_PLUGIN(QSvgPlugin)\n"
            "#endif\n",
            encoding="utf-8"
        )

    # --- Environment for qmake -------------------------------------------------
    qmake_env = prefer_clang64_in_path(env.copy()) if IS_WINDOWS else env.copy()

    # Add pkg-config paths so qmake can find pc files if it uses pkg-config
    qmake_env["PKG_CONFIG_PATH"] = os.pathsep.join(filter(None, [
        qmake_env.get("PKG_CONFIG_PATH", ""),
        str(deps_prefix / "lib" / "pkgconfig"),
        *(str(p) for p in pkgconfig_search)
    ]))

    # Add CMAKE_PREFIX_PATH too (some CMake-based checks in .pri may rely on it)
    qmake_env["CMAKE_PREFIX_PATH"] = os.pathsep.join(filter(None, [
        qmake_env.get("CMAKE_PREFIX_PATH", ""),
        str(deps_prefix)
    ]))

    # --- Compiler/link flags --------------------------------------------------
    cflags = (qmake_env.get("QMAKE_CFLAGS_RELEASE", "") + " " + GLOBAL_OPT_CFLAGS +
              " -ffunction-sections -fdata-sections").strip()
    cxxflags = (qmake_env.get("QMAKE_CXXFLAGS_RELEASE", "") + " " + GLOBAL_OPT_CXXFLAGS +
                " -ffunction-sections -fdata-sections").strip()
    ldflags = (qmake_env.get("QMAKE_LFLAGS_RELEASE", "") + " " + GLOBAL_LDFLAGS +
               " -Wl,--gc-sections -Wl,--icf=safe").strip()
    if lld_path.exists():
        ldflags += " -fuse-ld=lld"

    qmake_env["QMAKE_CFLAGS_RELEASE"] = cflags
    qmake_env["QMAKE_CXXFLAGS_RELEASE"] = cxxflags
    qmake_env["QMAKE_LFLAGS_RELEASE"] = ldflags

    # --- Helper: find static .a in search roots --------------------------------
    # Search order: deps_lib (local _install) -> lib_search_extra (msys clang64) -> system lib dirs
    def find_static_archive(basename_no_prefix: str, alt_filenames: list[str] = None) -> Path | None:
        """
        Try to find a static archive file for library named basename_no_prefix (e.g., 'z' or 'neurolabi').
        alt_filenames: optional list of candidate filenames (e.g., ['libpcre2-posix.a', 'libpcre2posix.a'])
        Returns Path to archive if found, else None.
        """
        candidates = []
        if alt_filenames:
            candidates.extend(alt_filenames)
        else:
            candidates.append(f"lib{basename_no_prefix}.a")
            # sometimes imported as name.a (without lib prefix) - include both
            candidates.append(f"{basename_no_prefix}.a")
        search_roots = [deps_lib] + lib_search_extra + [Path(p) for p in (qmake_env.get("LD_LIBRARY_PATH", "").split(os.pathsep) if qmake_env.get("LD_LIBRARY_PATH") else [])]
        # Also include default system lib dirs on non-Windows
        if not IS_WINDOWS:
            search_roots.extend([Path("/usr/lib"), Path("/usr/lib/x86_64-linux-gnu"), Path("/usr/local/lib")])
        for root in search_roots:
            if not root:
                continue
            for fname in candidates:
                p = Path(root) / fname
                if p.exists():
                    return p.resolve()
        return None

    # --- Required core static libs and their candidate names -------------------
    # Each item: (short_name_for_message, candidate_filenames)
    core_static_requirements = [
        ("libneurolabi", ["libneurolabi.a", "neurolabi.a"]),
        ("z (zlib)", ["libz.a", "zlib.a", "z.a"]),
        ("tiff", ["libtiff.a", "tiff.a"]),
        ("libxml2", ["libxml2.a", "xml2.a"]),
        ("jansson", ["libjansson.a", "jansson.a"]),
        ("pcre2-posix", ["libpcre2-posix.a", "pcre2-posix.a"]),
        ("pcre2-8", ["libpcre2-8.a", "pcre2-8.a"]),
        ("fftw3", ["libfftw3.a", "fftw3.a"]),
        ("lzma (xz)", ["liblzma.a", "lzma.a"]),
        ("iconv", ["libiconv.a", "iconv.a"]),
    ]

    # If user asked for single-precision FFTW, add fftw3f
    if getattr(args, "fftw_single", False):
        core_static_requirements.append(("fftw3f", ["libfftw3f.a", "fftw3f.a"]))

    # --- Validate presence of required headers for some libs -------------------
    header_checks = [
        ("libxml2", deps_include / "libxml2" / "libxml" / "parser.h"),
        ("zlib", deps_include / "zlib.h"),
        ("pcre2", deps_include / "pcre2posix.h"),
    ]

    missing = []
    found_libs = {}

    # Check headers first (fast fail)
    for name, hdr_path in header_checks:
        if not hdr_path.exists():
            # Try alternate include roots from lib_search_extra
            found = False
            for extra in lib_search_extra:
                alt = Path(extra) / "include" / hdr_path.name
                if alt.exists():
                    found = True
                    break
            if not found:
                missing.append(f"Header for {name} not found: expected {hdr_path} (check {deps_include} and MSYS2 clang64 include paths)")
    if missing:
        print("[gui] ERROR: Missing required headers:")
        for m in missing:
            print("   -", m)
        raise SystemExit("[gui] Aborting due to missing header(s).")

    # Now check static archives
    for msgname, candidates in core_static_requirements:
        found = find_static_archive("", candidates)
        if found:
            found_libs[msgname] = found
            print(f"[gui] Found {msgname} => {found}")
        else:
            # If we are on non-Windows (Linux) it's sometimes acceptable to link dynamic system libs,
            # but for robustness require static .a according to user's request. Offer hints.
            hint_pkg = None
            if msgname.startswith("libneurolabi"):
                hint_pkg = "Build libneurolabi (run the build script for core C)"
            elif msgname in ("z (zlib)", "libxml2", "tiff", "jansson", "pcre2-8", "pcre2-posix", "fftw3", "lzma (xz)"):
                hint_pkg = f"Make sure {msgname} static .a exists under {deps_lib} or install corresponding msys2 static package"
            else:
                hint_pkg = f"{msgname} static archive not found"
            print(f"[gui] ERROR: Required static archive for '{msgname}' not found. Searched in: {deps_lib} + {lib_search_extra} + system lib dirs.")
            if IS_WINDOWS:
                # Suggest pacman packages when possible (maps are suggestions)
                suggestion_map = {
                    "z (zlib)": "mingw-w64-clang-x86_64-zlib",
                    "tiff": "mingw-w64-clang-x86_64-libtiff",
                    "libxml2": "mingw-w64-clang-x86_64-libxml2",
                    "jansson": "mingw-w64-clang-x86_64-jansson",
                    "pcre2-posix": "mingw-w64-clang-x86_64-pcre2",
                    "pcre2-8": "mingw-w64-clang-x86_64-pcre2",
                    "fftw3": "mingw-w64-clang-x86_64-fftw",
                    "lzma (xz)": "mingw-w64-clang-x86_64-xz",
                    "iconv": "mingw-w64-clang-x86_64-libiconv",
                }
                pkg = suggestion_map.get(msgname, None)
                if pkg:
                    print(f"     Suggest: pacman -S --needed {pkg}")
            print("     Hint:", hint_pkg)
            # Fail immediately to avoid running expensive qmake / compile and failing mid-build
            raise SystemExit(f"[gui] Missing required static dependency: {msgname}")

    # --- Compose link line segments (prefer static) ----------------------------
    # Start with our _install lib dir then clang64 lib dir(s)
    libs_segments = [
        f"-L{deps_lib.as_posix()}",
    ]
    for extra in lib_search_extra:
        libs_segments.append(f"-L{extra.as_posix()}")

    # Add start-group and whole-archive for libneurolabi
    # Keep the consistent ordering: group static archives then switch to dynamic system libs
    libs_segments.append("-Wl,--start-group")
    # whole-archive only around neurolabi to force pulling all objects if needed
    libs_segments.append("-Wl,--whole-archive")
    libs_segments.append(f"-lneurolabi")
    libs_segments.append("-Wl,--no-whole-archive")

    # Add core static libs in a deterministic order
    core_order = ["z (zlib)", "tiff", "libxml2", "jansson", "pcre2-posix", "pcre2-8", "fftw3", "lzma (xz)", "iconv"]
    if getattr(args, "fftw_single", False):
        core_order.insert(core_order.index("fftw3") + 1, "fftw3f")
    for short in core_order:
        key = short
        # map display names to library tokens
        token_map = {
            "z (zlib)": "-lz",
            "tiff": "-ltiff",
            "libxml2": "-lxml2",
            "jansson": "-ljansson",
            "pcre2-posix": "-lpcre2-posix",
            "pcre2-8": "-lpcre2-8",
            "fftw3": "-lfftw3",
            "fftw3f": "-lfftw3f",
            "lzma (xz)": "-llzma",
            "iconv": "-liconv",
        }
        tok = token_map.get(short)
        if tok:
            libs_segments.append(tok)

    libs_segments.append("-Wl,--end-group")

    # --- System/dynamic runtimes after static group ----------------------------
    if IS_WINDOWS:
        libs_segments.extend([
            "-lopengl32", "-lglu32",
            "-lbcrypt", "-lws2_32", "-luser32", "-lkernel32", "-lgdi32", "-lwinmm", "-limm32",
            "-lshell32", "-lole32", "-loleaut32", "-luuid", "-ldwmapi", "-lversion", "-ladvapi32",
            "-lsetupapi", "-lcomdlg32", "-lcomctl32", "-lshlwapi"
        ])
    else:
        libs_segments.extend(["-lpthread", "-lm", "-ldl"])
        if sys.platform.startswith("linux"):
            libs_segments.extend(["-lGL", "-lGLU", "-lrt", "-lX11"])
        elif sys.platform == "darwin":
            # mac frameworks are handled in the .pro
            pass

    # Optionally add libc++ flags if qmake indicates clang + libc++ usage
    if "clang" in qmake_path.name.lower() or "clang" in qmake_env.get("CXX", ""):
        # Only add when necessary; harmless otherwise on windows where libc++ isn't used
        if IS_WINDOWS:
            # Only add if found in clang64 lib dir
            libcxx = find_static_archive("c++", ["libc++.a"])
            if libcxx:
                libs_segments.extend(["-lc++", "-lc++abi", "-lunwind"])

    # Convert to one LIBS token string for qmake (preserve spaces)
    libs_token = " ".join(libs_segments)

    # --- qmake command-line ---------------------------------------------------
    qmake_args = [
        str(qmake_path),
        "-o", "Makefile",
        str((gui_dir / "gui.pro").resolve()),
        "CONFIG+=release" if args.config == "release" else "CONFIG+=debug",
        "CONFIG+=static",
        "DEFINES+=QT_STATICPLUGIN",
        f'INCLUDEPATH+={deps_include.as_posix()}',
        f'INCLUDEPATH+={deps_include.as_posix()}/libxml2',
        f'LIBS+={libs_token}',
    ]

    if getattr(args, "edition", None) == "flyem":
        qmake_args.append('DEFINES+=_FLYEM_')
    elif getattr(args, "edition", None) == "biocytin":
        qmake_args.append('DEFINES+=_BIOCYTIN_')

    print("[gui] qmake args (tail):", " ".join(qmake_args[3:]))

    # Run qmake
    run_checked(qmake_args, cwd=app_build_dir, env=qmake_env, shell=False)

    # Find make tool
    make_tool = None
    for cand in candidate_makes:
        p = find_executable_on_path(cand)
        if p:
            make_tool = p
            break
    if not make_tool:
        raise RuntimeError("[gui] No make tool found (mingw32-make/make/ninja). Install one or ensure it is in PATH.")

    # Build
    run_checked([make_tool, f"-j{jobs_concurrency()}"], cwd=app_build_dir, env=qmake_env)

    # --- Collect artifacts ----------------------------------------------------
    produced_binary = None
    for p in app_build_dir.rglob("*"):
        if p.is_file():
            if IS_WINDOWS and p.suffix.lower() == ".exe" and "neu" in p.name.lower():
                produced_binary = p
                break
            if (not IS_WINDOWS) and os.access(p, os.X_OK) and "neu" in p.name.lower():
                produced_binary = p
                break
    if produced_binary:
        shutil.copy2(produced_binary, dist_dir / produced_binary.name)
        print(f"[gui] Built binary: {produced_binary.name}")
    else:
        print("[gui] WARNING: No produced GUI binary was detected in app build dir.")

    # --- Copy docs/configs ----------------------------------------------------
    doc_src = gui_dir / "doc"
    if doc_src.exists():
        dst = dist_dir / "doc"
        if dst.exists():
            shutil.rmtree(dst)
        shutil.copytree(doc_src, dst)

    # edition-specific copies
    if getattr(args, "edition", None) == "flyem":
        for src, dst in [(gui_dir / "config_flyem.xml", dist_dir / "config.xml"),
                         (gui_dir / "doc" / "flyem_proofread_help.html", dist_dir / "doc" / "shortcut.html")]:
            if src.exists():
                dst.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src, dst)
        json_dir = REPO_ROOT / "src" / "json"
        if json_dir.exists():
            dst = dist_dir / "json"
            if dst.exists():
                shutil.rmtree(dst)
            shutil.copytree(json_dir, dst)
    elif getattr(args, "edition", None) == "biocytin":
        src = gui_dir / "biocytin_config.xml"
        if src.exists():
            shutil.copy2(src, dist_dir / "config.xml")
    else:
        src = gui_dir / "config.xml"
        if src.exists():
            shutil.copy2(src, dist_dir / "config.xml")

    # --- Final summary --------------------------------------------------------
    print(f"[gui] Dist dir: {dist_dir}")
    print("[gui] Linked static libs from:", deps_lib)
    print("[gui] If linking fails, run the build with VERBOSE to inspect the final link line.")


# ---------- Clean helpers ----------
def remove_generated_c_templates():
    to_remove = [
        "tz_darray.c","tz_farray.c","tz_iarray.c","tz_i64array.c",
        "tz_dmatrix.c","tz_fmatrix.c","tz_imatrix.c",
        "tz_dimage_lib.c","tz_fimage_lib.c","tz_iimage_lib.c",
        "tz_fftw.c","tz_fftwf.c",
        "tz_int_arraylist.c","tz_int64_arraylist.c","tz_unipointer_arraylist.c",
        "tz_stack_tile_arraylist.c","tz_stack_tile_i_arraylist.c","tz_neuron_component_arraylist.c","tz_swc_arraylist.c",
        "tz_int_linked_list.c","tz_double_linked_list.c","tz_object_3d_linked_list.c","tz_voxel_linked_list.c",
        "tz_int_doubly_linked_list.c","tz_local_neuroseg_plane_doubly_linked_list.c",
        "tz_local_neuroseg_ellipse_doubly_linked_list.c","tz_local_neuroseg_doubly_linked_list.c",
        "tz_locseg_node.c","tz_locrect_node.c","tz_locnp_node.c",
        "tz_locseg_node_doubly_linked_list.c","tz_locrect_node_doubly_linked_list.c","tz_locnp_node_doubly_linked_list.c",
        "tz_locseg_chain_com.c","tz_locrect_chain_com.c","tz_locnp_chain_com.c","tz_locne_chain_com.c","tz_locne_node.c",
    ]
    src_c = (REPO_ROOT / "src" / "c")
    if not src_c.exists(): return
    for name in to_remove:
        p = src_c / name
        try:
            if p.exists():
                p.unlink()
                print(f"[distclean] removed {p}")
        except Exception as e:
            print(f"[distclean] could not remove {p}: {e}")


def pick_first_available(env: dict, names: list[str]) -> str | None:
    """
    Return first executable from names that exists in PATH (using shutil.which),
    respecting current env['PATH'].
    """
    # shutil.which reads the actual process PATH, so temporarily set os.environ?
    # We will use shutil.which with the PATH supplied in env by passing PATH in env to subprocess isn't available,
    # so we check manually using os.access on PATH entries:
    path = env.get("PATH", os.environ.get("PATH", ""))
    for name in names:
        # If name contains a path, check directly
        if os.path.isabs(name) and os.access(name, os.X_OK):
            return name
        # Try basic which-like search (works cross-platform)
        for d in path.split(os.pathsep):
            if not d:
                continue
            cand = os.path.join(d, name)
            if os.path.isfile(cand) and os.access(cand, os.X_OK):
                return cand
    return None

# ---------- CLI ----------
def main():
    parser = argparse.ArgumentParser(description="neuTube unified builder (MSYS2 clang64 on Windows, gcc/clang on Linux)")

    # Dirs
    parser.add_argument("--work-dir",        default=str((REPO_ROOT / "_work").resolve()))
    parser.add_argument("--build-dir",       default=str((REPO_ROOT / "_build").resolve()))
    parser.add_argument("--install-prefix",  default=str((REPO_ROOT / "_install").resolve()))

    # Feature toggles
    parser.add_argument("--fftw-single",     action="store_true", help="Also link single-precision FFTW (libfftw3f) from MSYS2 package")
    parser.add_argument("--edition",         default="general", choices=["general","flyem","biocytin"])
    parser.add_argument("--config",          default="release",  choices=["release","debug"])

    # What to build
    parser.add_argument("--only-deps", action="store_true")
    parser.add_argument("--only-src",  action="store_true")
    parser.add_argument("--only-gui",  action="store_true")

    # Clean/force
    parser.add_argument("--clean",       action="store_true", help="Remove _build/* and app build dirs")
    parser.add_argument("--distclean",   action="store_true", help="Remove _build, _work, _install, and app build dirs")
    parser.add_argument("--force-deps",  action="store_true", help="Force rebuild deps even if outputs exist")

    args = parser.parse_args()

    # Prepare environment for clang64
    base_env = prefer_clang64_in_path(os.environ.copy())
    cands = {
        "AR": ["llvm-ar", "ar", "gcc-ar", "llvm-ar.exe", "ar.exe"],
        "RANLIB": ["llvm-ranlib", "ranlib", "ranlib.exe", "llvm-ranlib.exe"],
        "NM": ["llvm-nm", "nm", "llvm-nm.exe", "nm.exe"],
        "STRIP": ["llvm-strip", "strip", "llvm-strip.exe", "strip.exe"],
    }

    for var, names in cands.items():
        # Respect an already set env var (don't override)
        if base_env.get(var):
            continue
        found = pick_first_available(base_env, names)
        if found:
            # Use the bare executable name if it was found by name (not absolute path)
            base_env[var] = os.path.basename(found)
            print(f"[env] {var} -> {base_env[var]}")
        else:
            print(f"[env] Warning: no candidate found for {var}; builds may pick system default (this is OK if you intend that).")

    # Basic sanity in clang64 shell
    if IS_WINDOWS:
        if not (find_executable_on_path("clang") and find_executable_on_path("clang++")):
            raise SystemExit("clang/clang++ not found. Make sure you’re in the *MSYS2 CLANG64* shell and the toolchain is installed.")
        if not find_executable_on_path("perl"):
            raise SystemExit("Perl not found. Install mingw-w64-clang-x86_64-perl in CLANG64.")
        if not Path(r"C:\msys64\clang64\bin\qmake.exe").exists():
            print("[hint] qmake not found in clang64. Install qt5-base / qt5-static for clang64.")
    else:
        # Linux: assume system compilers; the rest of the workflow is similar.
        pass

    if args.distclean:
        remove_generated_c_templates()
        for p in (Path(args.build_dir), Path(args.work_dir), Path(args.install_prefix),
                  REPO_ROOT / "_app_build_release", REPO_ROOT / "_app_build_debug", REPO_ROOT / "dist"):
            if p.exists():
                print(f"[distclean] Removing: {p}")
                shutil.rmtree(p, ignore_errors=True)
        print("[distclean] Done.")
        return
    if args.clean:
        bdir = Path(args.build_dir)
        if bdir.exists():
            print(f"[clean] Removing build dir: {bdir}")
            shutil.rmtree(bdir, ignore_errors=True)
        for p in (REPO_ROOT / "_app_build_release", REPO_ROOT / "_app_build_debug"):
            if p.exists():
                print(f"[clean] Removing app build dir: {p}")
                shutil.rmtree(p, ignore_errors=True)

    build_everything = not any([args.only_deps, args.only_src, args.only_gui])
    if args.only_deps or build_everything:
        build_static_dependencies(args, base_env)
    if args.only_src or build_everything:
        build_neutube_core_c(args, base_env)
    if args.only_gui or build_everything:
        build_neutube_gui(args, base_env)

    print("\nAll done.")

if __name__ == "__main__":
    sys.exit(main())
