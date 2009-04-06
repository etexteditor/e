#!/bin/bash

_download()
{
  # Download external libraries
  echo "Downloading external libraries..."
  echo
  pushd arch
  wget http://curl.haxx.se/download/curl-7.18.2.tar.gz
  wget http://libtomcrypt.com/files/crypt-1.11.tar.bz2
  wget http://math.libtomcrypt.com/files/ltm-0.39.tar.bz2
  wget http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz
  wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.6.tar.gz
  wget http://kent.dl.sourceforge.net/sourceforge/tinyxml/tinyxml_2_5_3.tar.gz
  popd
}

_backup_existing_patches()
{
  # Removing previous folders
  tempdir=`mktemp -d back.XXXXXX` || exit 1
  echo "Moving existing folders to $tempdir"
  mv curl $tempdir/curl
  mv libtomcrypt $tempdir/libtomcrypt
  mv libtommath $tempdir/libtommath
  mv metakit $tempdir/metakit
  mv pcre $tempdir/pcre
  mv tinyxml $tempdir/tinyxml
}

_extract_and_patch()
{
  _backup_existing_patches

  # Extract
  echo "Extracting libraries.."
  echo
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
}

_next_steps()
{
  echo "** The libraries have been installed. **"
  echo "Now you just need to build the following projects in msvc:"
  echo
  echo "curl\lib\curllib.sln (curllib)"
  echo "libtomcrypt\libtomcrypt.sln (libtomcrypt)"
  echo "libtommath\libtommath.sln (libtommath)"
  echo "metakit\win\msvc90\mksrc.sln (mklib)"
  echo "pcre\pcre.sln (pcre)"
  echo "tinyxml\tinyxml.sln (tinyxml)"
  echo
  echo "Run build_externals_win.cmd in a Visual Studio 2008 Command Prompt to compile from the command-line."
}


if [[ ! -e arch ]]; then
    mkdir arch
fi

if [[ "$1" != "repatch" ]]; then
      _download
fi

_extract_and_patch
_next_steps

exit 0
