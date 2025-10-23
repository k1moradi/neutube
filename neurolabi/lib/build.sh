#!/bin/bash

set -e

symlink_to_conda() {
    PKG_NAME=$1
    LIB_NAME=$2
    if [ ! -z "${CONDA_ENV}" ] && [ -f "${CONDA_ENV}/lib/${LIB_NAME}" ]; then
        echo "Creating conda symlinks for $PKG_NAME"
        mkdir -p $PKG_NAME
        cd $PKG_NAME
        ln -s -f "${CONDA_ENV}/include"
        ln -s -f "${CONDA_ENV}/lib"
        cd -
    fi
}

uncompress_lib () {
  local base="$1"
  if   [ -f "${base}.tar" ]; then
    tar -xvf "${base}.tar"
  elif [ -f "${base}.tar.gz" ] || [ -f "${base}.tgz" ]; then
    tar -xzvf "${base}.tar.gz" 2>/dev/null || tar -xzvf "${base}.tgz"
  elif [ -f "${base}.tar.xz" ] || [ -f "${base}.txz" ]; then
    tar -xJvf "${base}.tar.xz" 2>/dev/null || tar -xJvf "${base}.txz"
  else
    echo "uncompress_lib: no archive found for '${base}' (tried .tar, .tar.gz/.tgz, .tar.xz/.txz)" >&2
    return 1
  fi
}


libdir=`pwd`
export CFLAGS="-fPIC"


symlink_to_conda fftw3 libfftw3.a

if [ ! -f fftw3/lib/libfftw3.a ]
then
  if [ ! -d fftw3 ]
  then
    mkdir fftw3
  fi

  echo 'Building libfftw3 ...'
  uncompress_lib fftw-3.3.2
  cd fftw-3.3.2
  ./configure --enable-shared=no --with-pic --prefix=${libdir}/fftw3 --disable-fortran
  make
  make install

  ./configure --enable-shared=no --enable-float --with-pic --prefix=${libdir}/fftw3 --disable-fortran
  make
  make install
  cd ..
fi


symlink_to_conda jansson libjansson.a

if [ ! -f jansson/lib/libjansson.a ]
then
  echo 'Building libjansson ...'
  if [ ! -d jansson ]
  then
    mkdir jansson
  fi
  uncompress_lib jansson-2.5
  cd jansson-2.5
  ./configure --enable-shared=no --with-pic --prefix=${libdir}/jansson
  make
  make install
  cd ..
fi


symlink_to_conda xml libxml2.a

if [ ! -f xml/lib/libxml2.a ]
then
  echo 'Building libxml ...'
  if [ ! -d xml ]
  then
    mkdir xml
  fi
  uncompress_lib libxml2-2.9.14
  cd libxml2-2.9.14
  ./configure --without-iconv --without-zlib --without-lzma  --with-pic --enable-shared=no --prefix=${libdir}/xml --without-python
  make
  make install
  cd ..
fi

use_png=0
if [ $use_png -ne 0 ]
then

  symlink_to_conda png libpng.a

  if [ ! -f png/lib/libpng.a ]
  then
    echo 'Building libpng ...'
    if [ ! -d png ]
    then
      mkdir png
    fi
    uncompress_lib libpng-1.6.7
    cd libpng-1.6.7
    ./configure --enable-shared=no --with-pic --prefix=${libdir}/png
    make
    make install
    cd ..
  fi
fi


symlink_to_conda hdf5 libhdf5.a

use_hdf5=0
if [ $use_hdf5 -ne 0 ]
then
  if [ ! -f hdf5/lib/libhdf5.a ]
  then
    echo 'Building libhdf ...'
    if [ ! -d hdf5 ]
    then
      mkdir hdf5
    fi
    uncompress_lib hdf5-1.8.12
    cd hdf5-1.8.12
    ./configure --enable-shared=no --with-pic --prefix=${libdir}/hdf5
    make
    make install
    cd ..
  fi
fi

