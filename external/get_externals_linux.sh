#!/bin/bash

#FIXME: refactor windows/linux code

_download()
{
  # Download external libraries
  echo "Downloading external libraries..."
  echo
  pushd arch
  wget http://libtomcrypt.com/files/crypt-1.11.tar.bz2
  wget http://math.libtomcrypt.com/files/ltm-0.39.tar.bz2
  wget http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz
  wget ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.6.tar.gz
  wget http://kent.dl.sourceforge.net/sourceforge/tinyxml/tinyxml_2_5_3.tar.gz
  wget http://biolpc22.york.ac.uk/pub/2.8.9/wxWidgets-2.8.9.tar.bz2
  wget http://builds.nightly.webkit.org/files/trunk/src/WebKit-r42260.tar.bz2
  popd
}

_backup_existing_patches()
{
  # Removing previous folders
  tempdir=`mktemp -d back.XXXXXX` || exit 1
  echo "Moving existing folders to $tempdir"
  mv libtomcrypt $tempdir/libtomcrypt
  mv libtommath $tempdir/libtommath
  mv metakit $tempdir/metakit
  mv pcre $tempdir/pcre
  mv tinyxml $tempdir/tinyxml
}

_extract_and_patch()
{
  # Extract
  echo "Extracting libraries.."
  echo
  tar -xjf arch/crypt-*
  tar -xjf arch/ltm-*
  tar -xzf arch/metakit-*
  tar -xzf arch/pcre-*
  tar -xzf arch/tinyxml_*
  tar -xjf arch/wxWidgets-*
  tar -xjf arch/WebKit-*

  # Rename directories to generic names
  echo "Renaming dirs..."
  echo
  mv libtomcrypt-* libtomcrypt
  mv libtommath-* libtommath
  mv metakit-* metakit
  mv pcre-* pcre
  mv wxWidgets-* wxwidgets
  mv WebKit-* webkit

  # Apply patches
  echo "Applying patches..."
  echo
  patch -Np1 -d metakit < patches/metakit.patch
  patch -d pcre < patches/pcre.patch
  patch tinyxml/tinyxml.cpp < patches/tinyxml/tinyxml.cpp.patch
  patch tinyxml/tinyxml.h < patches/tinyxml/tinyxml.h.patch
  patch -Np0 -d webkit < patches/webkit.patch
  patch wxwidgets/src/aui/auibook.cpp < patches/wxwidgets/auibook.cpp.patch
  patch wxwidgets/include/wx/aui/auibook.h < patches/wxwidgets/auibook.h.patch
}

_next_steps()
{
  echo "** The libraries have been installed. **"
  echo "Now you just need to run build_externals_linux.sh"
}


if [[ ! -e arch ]]; then
    mkdir arch
fi

if [[ "$1" != "repatch" ]]; then
      _download
fi

_backup_existing_patches
_extract_and_patch
_next_steps

exit 0
