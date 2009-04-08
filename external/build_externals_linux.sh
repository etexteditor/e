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
./configure --prefix=$output --enable-shared=no --enable-static=yes --enable-utf8 --enable-unicode-properties --enable-newline-is-any
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
CFLAGS="-I../out/include -DLTC_NO_ROLC -DLTM_DESC" make
make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtomcrypt/pdf NODOCS=1
popd

# wxwidgets
pushd wxwidgets
./configure --prefix=$output --enable-monolithic=yes --enable-shared=no --enable-unicode --enable-ffile \
                     --without-libtiff --enable-graphics_ctx \
                     --disable-largefile CFLAGS="$CFLAGS -D_FILE_OFFSET_BITS=32" # ecore was built agains non-largefile aware wxWidgets so our wxWidgets has to be configured same way
make
make install
popd

# wxwebkit
pushd webkit
PATH="$output/bin:${PATH}" ./WebKitTools/Scripts/build-webkit --wx wxgc --no-svg --no-coverage --no-workers \
      --no-video --no-wml --no-database --no-icon-database --no-xpath --no-xslt --prefix=$output --enable-static --disable-shared
popd

