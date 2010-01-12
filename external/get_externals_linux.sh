#!/bin/bash

#FIXME: refactor windows/linux code

_download()
{
  # Download external libraries
  echo "Downloading external libraries..."
  echo
  pushd arch
  wget -nc http://libtomcrypt.com/files/crypt-1.11.tar.bz2
  wget -nc http://math.libtomcrypt.com/files/ltm-0.39.tar.bz2
  wget -nc http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz
  wget -nc ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.9.tar.gz
  wget -nc http://kent.dl.sourceforge.net/sourceforge/tinyxml/tinyxml_2_5_3.tar.gz
  wget -nc http://biolpc22.york.ac.uk/pub/2.8.10/wxWidgets-2.8.10.tar.bz2
  wget -nc http://builds.nightly.webkit.org/files/trunk/src/WebKit-r43163.tar.bz2
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
	echo "---- Applying metakit patches ----"
  patch -Np1 -d metakit < patches/metakit.patch
	echo "---- Applying tinyxml patches ----"
  patch tinyxml/tinyxml.cpp < patches/tinyxml/tinyxml.cpp.patch
  patch tinyxml/tinyxml.h < patches/tinyxml/tinyxml.h.patch
	echo "---- Applying wxwidget patches ----"
  patch -Np1 -d wxwidgets < patches/wxWidgets-gsock.patch
	echo "---- Applying webkit patches ----"
  patch -Np1 -d webkit < patches/webkit/remove-targets.patch
  patch -Np1 -d webkit < patches/webkit/fully-static.patch
  patch -Np0 -d webkit < patches/webkit/vis_hidden.patch
  patch -Np1 -d webkit < patches/webkit/cancelledError.patch
  patch -Np1 -d webkit < patches/webkit/local_sec.patch
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
