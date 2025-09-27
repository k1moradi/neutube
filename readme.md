# 0) Sync and fully update the system (the shell may ask you to restart once)
pacman -Syu

pacman -S --needed mingw-w64-clang-x86_64-clang mingw-w64-clang-x86_64-lld mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-ninja mingw-w64-clang-x86_64-pkg-config git curl unzip zip patch mingw-w64-clang-x86_64-qt5-static mingw-w64-clang-x86_64-qt5-tools mingw-w64-clang-x86_64-qt5-svg mingw-w64-clang-x86_64-qt5-imageformats mingw-w64-clang-x86_64-fftw perl


# 1) Core toolchain + build tools for CLANG64
pacman -S --needed mingw-w64-clang-x86_64-clang mingw-w64-clang-x86_64-lld mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-ninja mingw-w64-clang-x86_64-pkg-config git curl unzip zip patch 

# 2) Static Qt5 (prebuilt) for CLANG64
pacman -S --needed mingw-w64-clang-x86_64-qt5-static mingw-w64-clang-x86_64-qt5-tools mingw-w64-clang-x86_64-qt5-svg mingw-w64-clang-x86_64-qt5-imageformats

# 3) FFTW3 for CLANG64 (provides static libs too)
pacman -S --needed mingw-w64-clang-x86_64-fftw

# 4) Other C deps you still build from source need headers to be discoverable at link time.
#    If you decide later to use prebuilt ones instead of building them, install:
# pacman -S --needed mingw-w64-clang-x86_64-zlib \
#                    mingw-w64-clang-x86_64-libxml2 \
#                    mingw-w64-clang-x86_64-libtiff \
#                    mingw-w64-clang-x86_64-jansson \
#                    mingw-w64-clang-x86_64-pcre2

# 5) Perl for your *.t template generation (MSYS perl is fine now that Qt is prebuilt)
pacman -S --needed perl

6) make
pacman -S --needed mingw-w64-clang-x86_64-make


pacman -S --needed mingw-w64-clang-x86_64-libiconv mingw-w64-clang-x86_64-xz