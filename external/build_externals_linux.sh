#!/bin/bash

output=`pwd`/out
CFLAGS="-O2"

echo "Going to place output in $output"

if [[ ! -e $output ]]; then
    mkdir -p $output
fi

# Metakit
pushd metakit
cd builds
../unix/configure --prefix=$output --enable-shared=no CXXLAGS="$CFLAGS"
make
make install
popd

# PCRE
pushd pcre
./configure --prefix=$output --enable-shared=no --enable-static=yes
make
make install
popd

# tinyxml
pushd tinyxml
make TINYXML_USE_STL=NO
popd

# libtommath
pushd libtommath
make
make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtommath/pdf
popd

# libtomcrypt
pushd libtomcrypt
CC=${CC:-gcc}
make CC="$CC -DLTM_DESC -I../libtomcrypt" # unfortunately CFLAGS cannot be used as it will break the compilation
make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtomcrypt/pdf NODOCS=1
popd

# wxwidgets
pushd wxwidgets
./configure --prefix=$output --enable-monolithic=yes --enable-shared=no --disable-largefile CFLAGS="$CFLAGS -D_FILE_OFFSET_BITS=32" # ecore was built agains non-largefile aware wxWidgets so our wxWidgets has to be configured same way
make
make install
popd

