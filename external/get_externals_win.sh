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
    http://curl.haxx.se/download/curl-7.18.2.tar.gz \
    http://github.com/ajpalkovic/e/raw/master/external/downloads/crypt-1.11.tar.bz2 \
    http://github.com/ajpalkovic/e/raw/master/external/downloads/ltm-0.39.tar.bz2 \
    http://www.equi4.com/pub/mk/metakit-2.4.9.7.tar.gz \
    ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-7.9.tar.gz \
    http://downloads.sourceforge.net/project/tinyxml/tinyxml/2.5.3/tinyxml_2_5_3.tar.gz \
    http://downloads.sourceforge.net/project/wxwindows/wxAll/2.8.10/wxWidgets-2.8.10.tar.bz2 \
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
  echo "Backing up patched externals..."
  for dir in  curl libtomcrypt libtommath metakit prce tinyxml wxwidgets boost
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
  tar -xzf $downloads/curl-*
  tar -xjf $downloads/crypt-*
  tar -xjf $downloads/ltm-*
  tar -xzf $downloads/metakit-*
  tar -xzf $downloads/pcre-*
  tar -xzf $downloads/tinyxml_*
  tar -xjf $downloads/wxWidgets-*
  tar -xzf $downloads/boost_*

  # Rename directories to generic names
  echo "Renaming dirs..."
  echo

  mv curl-* curl
  mv libtomcrypt-* libtomcrypt
  mv libtommath-* libtommath
  mv metakit-* metakit
  mv pcre-* pcre
  mv wxWidgets-* wxwidgets
  mv boost_* boost

  # Apply patches
  echo "Applying patches..."
  echo
  patch -d libtomcrypt/src/headers < patches/libtomcrypt.patch
  patch -Np1 -d metakit < patches/metakit.patch
  patch tinyxml/tinyxml.cpp < patches/tinyxml/tinyxml.cpp.patch
  patch tinyxml/tinyxml.h < patches/tinyxml/tinyxml.h.patch

  # Copy msvc specific project files
  echo "Copying msvc specific project files..."
  echo
  cp build_msvc/curllib* curl/lib
  cp build_msvc/libtomcrypt* libtomcrypt
  cp build_msvc/libtommath* libtommath
  cp build_msvc/tinyxml/* tinyxml
  cp -r build_msvc/metakit/* metakit/win
  cp -r build_msvc/pcre/* pcre
}

_next_steps()
{
  echo "** The libraries have been downloaded and patched. **"
  echo "Now build the following projects in Visual Studio:"
  echo
  echo "curl\lib\curllib.sln"
  echo "libtomcrypt\libtomcrypt.sln"
  echo "libtommath\libtommath.sln"
  echo "metakit\win\msvc90\mksrc.sln"
  echo "pcre\pcre.sln"
  echo "tinyxml\tinyxml.sln"
  echo "wxwidgets\build\msw\wx.dsw"
  echo
  echo "You must also manually compile boost.  build_externals_win.cmd will do this for you."
  echo
  echo "For an automated build, run build_externals_win.cmd in a Visual Studio 2008 Command Prompt."
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
