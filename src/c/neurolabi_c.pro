# neurolabi/c/neurolabi_c.pro
TEMPLATE = lib
CONFIG  += staticlib warn_on
CONFIG  -= shared dll

# Output name (debug suffix to match gui’s expectation)
TARGET = neurolabi
CONFIG(debug, debug|release) {
    TARGET = neurolabi_debug
}

# Where the .lib goes
DESTDIR = $$PWD/lib

# Headers live here
INCLUDEPATH += $$PWD/include

# Windows/MSVC-friendly defines
win32-msvc {
    DEFINES += WIN32 _ENABLE_EXTENDED_ALIGNED_STORAGE _CRT_SECURE_NO_WARNINGS
}

# Source files (adjust the globs if your layout differs)
# Recursively pick up all .c and .cpp in c/src
SOURCES += $$files($$PWD/src/*.c, true) \
           $$files($$PWD/src/*.cpp, true)

HEADERS += $$files($$PWD/include/*.h, true)
