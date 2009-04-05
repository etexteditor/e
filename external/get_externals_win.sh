#!/bin/bash

mkdir arch
cd arch

# Download external libraries
echo "Downloading external libraries..."
echo
wget http://curl.haxx.se/download/curl-7.18.2.tar.gz
wget http://libtomcrypt.com/files/crypt-1.11.tar.bz2
wget http://math.libtomcrypt.com/files/ltm-0.39.tar.bz2
wget http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz
wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.6.tar.gz
wget http://kent.dl.sourceforge.net/sourceforge/tinyxml/tinyxml_2_5_3.tar.gz

# Extract
echo "Extracting libraries.."
echo
cd ..
tar -xzf arch/curl-*
tar -xjf arch/crypt-*
tar -xjf arch/ltm-*
tar -xzf arch/metakit-*
tar -xzf arch/pcre-*
tar -xzf arch/tinyxml_*

# Rename directories to generic names
echo "Renaming dirs..."
echo
mv curl-* curl
mv libtomcrypt-* libtomcrypt
mv libtommath-* libtommath
mv metakit-* metakit
mv pcre-* pcre

# Apply patches
echo "Applying patches..."
echo
patch -d libtomcrypt/src/headers < patches/libtomcrypt.patch
patch -Np1 -d metakit < patches/metakit.patch
patch -d pcre < patches/pcre.patch
patch tinyxml/tinyxml.cpp < patches/tinyxml/tinyxml.cpp.patch
patch tinyxml/tinyxml.h < patches/tinyxml/tinyxml.h.patch

# Copy msvc specific project files
echo "Copying msvc specific project files..."
echo
cp build_msvc/curllib* curl/lib
cp build_msvc/libtomcrypt* libtomcrypt
cp build_msvc/libtommath* libtommath
cp build_msvc/tinyxml* tinyxml
cp -r build_msvc/metakit/* metakit/win
cp -r build_msvc/pcre/* pcre

echo "** The libraries have been installed. **"
echo "Now you just need to build the following projects in msvc:"
echo
echo "curl\lib\curllib.sln (curllib)"
echo "libtomcrypt\libtomcrypt.sln (libtomcrypt)"
echo "libtommath\libtommath.sln (libtommath)"
echo "metakit\win\msvc90\mksrc.sln (mklib)"
echo "pcre\pcre.sln (pcre)"
echo "tinyxml\tinyxml.sln (tinyxml)"


