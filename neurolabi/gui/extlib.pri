# ========== Import env → qmake vars ==========
isEmpty(DVIDCPP_PATH): DVIDCPP_PATH = $$(DVIDCPP_PATH)
isEmpty(BUILDEM_DIR):  BUILDEM_DIR  = $$(BUILDEM_DIR)
isEmpty(CONDA_ENV):    CONDA_ENV    = $$(CONDA_PREFIX)
isEmpty(CONDA_ENV):    CONDA_ENV    = $$(CONDA_ENV)
isEmpty(VCPKG_ROOT):   VCPKG_ROOT   = $$(VCPKG_ROOT)
isEmpty(NEUROLABI_DIR): NEUROLABI_DIR = $$(NEUROLABI_DIR)

# Default NEUROLABI_DIR (this file is in neurolabi/gui)
isEmpty(NEUROLABI_DIR): NEUROLABI_DIR = $$clean_path($$PWD/..)

# Sentinels so exists() never sees empty
isEmpty(DVIDCPP_PATH): DVIDCPP_PATH = __UNSET__
isEmpty(BUILDEM_DIR):  BUILDEM_DIR  = __UNSET__
isEmpty(CONDA_ENV):    CONDA_ENV    = __UNSET__
isEmpty(VCPKG_ROOT):   VCPKG_ROOT   = __UNSET__
isEmpty(NEUROLABI_DIR): NEUROLABI_DIR = __UNSET__

# ========== Core includes ==========
EXTLIB_DIR  = $${NEUROLABI_DIR}/lib
DEPENDPATH += . $${NEUROLABI_DIR}/c $${NEUROLABI_DIR}/c/include
INCLUDEPATH += \
    $${NEUROLABI_DIR}/gui \
    $${NEUROLABI_DIR}/c \
    $${NEUROLABI_DIR}/c/include \
    $${EXTLIB_DIR}/genelib/src \
    $${NEUROLABI_DIR}/gui/ext

# ========== neurolabi core lib ==========
LIBS += -L$${NEUROLABI_DIR}/c/lib
CONFIG(debug, debug|release) {
    LIBS += -lneurolabi_debug
} else {
    LIBS += -lneurolabi
}

# Track vendor choice
TP_VENDOR = none

# ========== Prefer Conda if present ==========
equals(TP_VENDOR, none): !equals(CONDA_ENV, __UNSET__) {
    win32 {
        CONDA_INC = $${CONDA_ENV}/Library/include
        CONDA_LIB = $${CONDA_ENV}/Library/lib
        exists($$CONDA_INC/jansson.h) {
            TP_VENDOR = conda
            INCLUDEPATH += $$CONDA_INC $$CONDA_INC/libxml2
            LIBS       += -L$$CONDA_LIB -llibxml2 -ljansson -lfftw3 -lfftw3f
            message(3rdparty: using Conda at $$CONDA_ENV)
        }
    } else {
        CONDA_INC = $${CONDA_ENV}/include
        CONDA_LIB = $${CONDA_ENV}/lib
        exists($$CONDA_INC/jansson.h) {
            TP_VENDOR = conda
            INCLUDEPATH += $$CONDA_INC $$CONDA_INC/libxml2
            LIBS       += -L$$CONDA_LIB -lxml2 -ljansson -lfftw3 -lfftw3f
            message(3rdparty: using Conda at $$CONDA_ENV)
        }
    }
}

# ========== Fall back to vcpkg (dynamic → static) ==========
equals(TP_VENDOR, none): !equals(VCPKG_ROOT, __UNSET__) {
    VCPKG_D = $${VCPKG_ROOT}/installed/x64-windows
    exists($$VCPKG_D/include/jansson.h) {
        TP_VENDOR = vcpkg-dynamic
        INCLUDEPATH += $$VCPKG_D/include $$VCPKG_D/include/libxml2
        LIBS       += -L$$VCPKG_D/lib -llibxml2 -ljansson -lfftw3 -lfftw3f
        message(3rdparty: using vcpkg dynamic at $$VCPKG_D)
    }
}

equals(TP_VENDOR, none): !equals(VCPKG_ROOT, __UNSET__) {
    VCPKG_S = $${VCPKG_ROOT}/installed/x64-windows-static
    exists($$VCPKG_S/include/jansson.h) {
        TP_VENDOR = vcpkg-static
        INCLUDEPATH += $$VCPKG_S/include $$VCPKG_S/include/libxml2
        LIBS       += -L$$VCPKG_S/lib -llibxml2 -ljansson -lfftw3 -lfftw3f
        DEFINES    += LIBXML_STATIC
        win32-msvc: LIBS += -lws2_32 -lbcrypt -luser32 -lshell32 -lversion -liconv -llzma -lz
        message(3rdparty: using vcpkg static at $$VCPKG_S)
    }
}

# ========== Legacy packaged tree under EXTLIB_DIR ==========
equals(TP_VENDOR, none) {
    # Windows legacy bundle
    win32 {
        LEG_INC = $${EXTLIB_DIR}/msvc/64/include
        LEG_LIB = $${EXTLIB_DIR}/msvc/64/lib
        exists($$LEG_INC/jansson.h) {
            TP_VENDOR = extlib-msvc
            INCLUDEPATH += $$LEG_INC $$LEG_INC/libxml2
            LIBS       += -L$$LEG_LIB -llibxml2 -ljansson -lfftw3 -lfftw3f
            message(3rdparty: using EXTLIB_DIR (msvc) at $$EXTLIB_DIR)
        }
    }
    # Unix legacy bundle
    unix:equals(TP_VENDOR, none) {
        exists($${EXTLIB_DIR}/jansson/include/jansson.h) {
            TP_VENDOR = extlib-unix
            INCLUDEPATH += \
                $${EXTLIB_DIR}/jansson/include \
                $${EXTLIB_DIR}/xml/include/libxml2 \
                $${EXTLIB_DIR}/fftw3/include
            LIBS += \
                -L$${EXTLIB_DIR}/jansson/lib \
                -L$${EXTLIB_DIR}/xml/lib \
                -L$${EXTLIB_DIR}/fftw3/lib \
                -ljansson -lxml2 -lfftw3 -lfftw3f
            message(3rdparty: using EXTLIB_DIR (unix) at $$EXTLIB_DIR)
        }
    }
}

# ========== System paths / pkg-config (Linux/macOS) ==========
unix:equals(TP_VENDOR, none) {
    CONFIG += link_pkgconfig
    PKGCONFIG += jansson libxml-2.0 fftw3
    # Common prefixes, added opportunistically
    exists(/usr/include/jansson.h):        INCLUDEPATH += /usr/include
    exists(/usr/local/include/jansson.h):  INCLUDEPATH += /usr/local/include
    exists(/opt/local/include/jansson.h):  INCLUDEPATH += /opt/local/include
    exists(/opt/homebrew/include/jansson.h): INCLUDEPATH += /opt/homebrew/include
    exists(/usr/include/libxml2/libxml/parser.h):       INCLUDEPATH += /usr/include/libxml2
    exists(/usr/local/include/libxml2/libxml/parser.h): INCLUDEPATH += /usr/local/include/libxml2
    exists(/opt/local/include/libxml2/libxml/parser.h): INCLUDEPATH += /opt/local/include/libxml2
    exists(/opt/homebrew/include/libxml2/libxml/parser.h): INCLUDEPATH += /opt/homebrew/include/libxml2
    LIBS += -ljansson -lxml2 -lfftw3 -lfftw3f
    TP_VENDOR = system
    message(3rdparty: using system/pkg-config)
}

# ========== MSYS2 MinGW (Windows) ==========
win32-g++:equals(TP_VENDOR, none) {
    MSYS64 = C:/msys64/mingw64
    exists($$MSYS64/include/jansson.h) {
        TP_VENDOR = msys2-mingw
        INCLUDEPATH += $$MSYS64/include $$MSYS64/include/libxml2
        LIBS += -L$$MSYS64/lib -lxml2 -ljansson -lfftw3 -lfftw3f
        DEFINES += LIBXML_STATIC
        LIBS += -lpcre2posix -lpcre2-8 -lpthread -lz
        message(3rdparty: using MSYS2 MinGW at $$MSYS64)
    }
}

equals(TP_VENDOR, none) {
    message(3rdparty: WARNING no external vendor detected automatically)
}

# ========== Platform extras ==========
unix {
    # Optional HDF5 (static in your tree)
    exists($${EXTLIB_DIR}/hdf5/lib/libhdf5.a) {
        DEFINES += _ENABLE_HDF5_
        INCLUDEPATH += $${EXTLIB_DIR}/hdf5/include
        LIBS += -L$${EXTLIB_DIR}/hdf5/lib -lhdf5 -lhdf5_hl
    }
    LIBS += -ldl -lz
}

win32-msvc {
    QMAKE_CXXFLAGS += /wd4996
    # If migrating ancient code, this can help temporarily:
    # QMAKE_CXXFLAGS += /Zc:strictStrings-
}

# ========== Optional OpenCV ==========
CONFIG(debug, debug|release) {
    exists($${EXTLIB_DIR}/opencv) {
        DEFINES += _USE_OPENCV_
        INCLUDEPATH += $${EXTLIB_DIR}/opencv/include $${EXTLIB_DIR}/opencv/include/opencv
        LIBS += -L$${EXTLIB_DIR}/opencv/lib -lopencv_core -lopencv_ml
        message(opencv: using EXTLIB_DIR bundle)
    } else:unix:exists(/usr/include/opencv4/opencv2/core.hpp) {
        DEFINES += _USE_OPENCV_
        INCLUDEPATH += /usr/include/opencv4
        LIBS += -lopencv_core -lopencv_ml
        message(opencv: using system /usr/include/opencv4)
    }
}

# ========== Optional libdvidcpp ==========
contains(TEMPLATE, app) {
    !equals(DVIDCPP_PATH, __UNSET__) {
        DEFINES += _ENABLE_LIBDVIDCPP_ _LIBDVIDCPP_OLD_
        INCLUDEPATH += $$DVIDCPP_PATH/include
        LIBS += -L$$DVIDCPP_PATH/lib
        message(libdvidcpp: using DVIDCPP_PATH)
    } else: !equals(BUILDEM_DIR, __UNSET__) {
        INCLUDEPATH +=  $${BUILDEM_DIR}/include
        LIBS += -L$${BUILDEM_DIR}/lib -L$${BUILDEM_DIR}/lib64
        DEFINES += _ENABLE_LIBDVIDCPP_
        message(libdvidcpp: using BUILDEM_DIR)
    } else: !equals(CONDA_ENV, __UNSET__) {
        win32 {
            INCLUDEPATH += $${CONDA_ENV}/Library/include
            LIBS       += -L$${CONDA_ENV}/Library/lib
        } else {
            INCLUDEPATH += $${CONDA_ENV}/include
            LIBS       += -L$${CONDA_ENV}/lib
        }
        DEFINES += _ENABLE_LIBDVIDCPP_
        message(libdvidcpp: using Conda)
    }

    contains(DEFINES, _ENABLE_LIBDVIDCPP_) {
        LIBS *= -ldvidcpp -ljsoncpp -llz4 -lpng -lcurl -ljpeg -lboost_system -lboost_thread
        !contains(DEFINES, _LIBDVIDCPP_OLD_) {
            LIBS *= -lssl -lcrypto
        }
    } else: exists($${EXTLIB_DIR}/png/lib) {
        LIBS += -L$${EXTLIB_DIR}/png/lib -lpng
    }
}

# ========== Debug prints ==========
message(defs: $$DEFINES)
message(libs: $$LIBS)
message(inc : $$INCLUDEPATH)
