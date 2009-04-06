#!/bin/bash

output=`pwd`/out
echo "Going to place output in $output"

if [[ ! -e $output ]]; then
    mkdir -p $output
fi

# CURL
pushd curl
./configure --prefix=$output --disable-ldap --enable-shared=no --enable-static=yes
make
make install
popd

# libtomcrypt
#pushd libtomcrypt
#make DESTDIR=$output
#make install
#popd

# Metakit
pushd metakit
cd builds
../unix/configure --prefix=$output 
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
make
popd

# libtommath
pushd libtommath
make
make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtommath/pdf
popd

# libtomcrypt
pushd libtomcrypt
make
make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include NODOCS=1
popd

