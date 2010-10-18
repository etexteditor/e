#!/bin/bash

#FIXME: refactor windows/linux code

downloads=downloads

_download()
{
  # Download external libraries
  echo "Downloading external libraries..."
  echo
  pushd $downloads

  # Only download files we don't already have.
  # This lets us delete files to redownload, or get
  # new versions automatically.

  for url in \
    http://github.com/ajpalkovic/e/raw/master/external/downloads/crypt-1.11.tar.bz2 \
    http://github.com/ajpalkovic/e/raw/master/external/downloads/ltm-0.39.tar.bz2 \
    http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz \
    ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.9.tar.gz \
    http://kent.dl.sourceforge.net/sourceforge/tinyxml/tinyxml_2_5_3.tar.gz \
    http://biolpc22.york.ac.uk/pub/2.8.10/wxWidgets-2.8.10.tar.bz2 \
    http://builds.nightly.webkit.org/files/trunk/src/WebKit-r43163.tar.bz2 \
    http://downloads.sourceforge.net/project/boost/boost/1.44.0/boost_1_44_0.tar.gz
  do
    if [[ ! -e `basename $url` ]]; then
      curl -O -L $url
    fi
  done

  popd
}

_backup_existing_patches()
{
  # Removing previous folders
  tempdir=`mktemp -d back.XXXXXX` || exit 1
  echo "Moving existing folders to $tempdir"
  for dir in libtomcrypt libtommath metakit pcre tinyxml wxwidgets webkit boost
  do
    if [[ -e $dir ]]; then
      mv $dir $tempdir/$dir;
    fi
  done
}

_extract_and_patch()
{
  # Extract
  echo "Extracting libraries.."
  echo
  tar -xjf $downloads/crypt-*
  tar -xjf $downloads/ltm-*
  tar -xzf $downloads/metakit-*
  tar -xzf $downloads/pcre-*
  tar -xzf $downloads/tinyxml_*
  tar -xjf $downloads/wxWidgets-*
  tar -xjf $downloads/WebKit-*
  tar -xzf $downloads/boost_*

  # Rename directories to generic names
  echo "Renaming dirs..."
  echo
  mv libtomcrypt-* libtomcrypt
  mv libtommath-* libtommath
  mv metakit-* metakit
  mv pcre-* pcre
  mv wxWidgets-* wxwidgets
  mv WebKit-* webkit
  mv boost_* boost

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
  patch -Np1 -d webkit < patches/webkit/icu-fix-56345.diff
  patch -Np1 -d webkit < patches/webkit/wxtimerfix-gcc-4.5.diff
}

_next_steps()
{
  echo "** The libraries have been installed. **"
  echo "Now you just need to run build_externals_linux.sh"
}


if [[ ! -e $downloads ]]; then
    mkdir $downloads
fi

if [[ "$1" != "repatch" ]]; then
      _download
fi

_backup_existing_patches
_extract_and_patch
_next_steps

exit 0
