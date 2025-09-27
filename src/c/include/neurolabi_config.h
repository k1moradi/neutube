#ifndef NEUROLABI_CONFIG_H
#define NEUROLABI_CONFIG_H

/* Platform */
#define NEUROLABI_WINDOWS 1

/* C/C++ headers available with MSVC 2019+ */
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 0
#define HAVE_UNISTD_H 0
#define HAVE_SYS_TYPES_H 0

/* Threads: use Win32, not pthreads */
#define HAVE_PTHREAD 0

/* Libraries you plan to link (tune as you add deps) */
#define HAVE_LIBXML2 1
#define HAVE_FFTW3   1
#define HAVE_JANSSON 1

/* Endianness (Windows/x64 is little-endian) */
#define WORDS_BIGENDIAN 0

#endif /* NEUROLABI_CONFIG_H */
