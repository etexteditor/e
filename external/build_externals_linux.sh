#!/bin/bash

if [ x"$1" == x"release" ] ; then
    echo "Building release binaries"
    output=`pwd`/out.release
    cfg_switches=--disable-debug
    tinyxml_switches="DEBUG=YES"
    CPPFLAGS="-O2"
    LDFLAGS="-g"
elif [ x"$1" == x"debug" ] ; then
    echo "Building debug binaries"
    output=`pwd`/out.debug
    tinyxml_switches=
    cfg_switches=--enable-debug
    CPPFLAGS="-O0 -g"
    LDFLAGS="-g"
else
    echo "Usage: $0 [release|debug]\n";
    exit 1
fi

LBITS=`getconf LONG_BIT`
if [ x$LBITS == x"64" ] ; then
    echo "Building 64-bit binaries"
    WXWIDGETS_EXTRA_CFLAGS=
elif [ x$LBITS == x"32" ] ; then
    echo "Building 32-bit binaries"
    WXWIDGETS_EXTRA_CFLAGS="-D_FILE_OFFSET_BITS=32"
else
    echo "Cannot determine target architecture"
    exit 1
fi

echo "Going to place output in $output"

if [[ ! -e $output ]]; then
    mkdir -p $output
fi

# Metakit
pushd metakit
cd builds &&
    ../unix/configure --prefix=$output $cfg_switches --enable-shared=no CFLAGS="$CPPFLAGS" CXXFLAGS="$CPPFLAGS" &&
    make clean &&
    make &&
    make install ||
        ( echo "Cannot compile MetaKit" ; exit 1 )
popd

# PCRE
pushd pcre
./configure --prefix=$output --enable-shared=no --enable-static=yes --enable-utf8 --enable-unicode-properties --enable-newline-is-any CFLAGS="$CPPFLAGS" CXXFLAGS="$CPPFLAGS" &&
    make clean &&
    make &&
    make install &&
    cp config.h ucp.h $output/include ||
        ( echo "Cannot compile pcre" ; exit 1 )
popd

# tinyxml
pushd tinyxml
make clean &&
    make TINYXML_USE_STL=NO $tinyxml_switches CFLAGS="$CPPFLAGS" CXXFLAGS="$CPPFLAGS" &&
    ar r ./libtinyxml.a tinystr.o tinyxml.o xmltest.o tinyxmlerror.o tinyxmlparser.o &&
    ranlib ./libtinyxml.a &&
    cp -f ./libtinyxml.a $output/lib/ &&
    cp -f ./*.h $output/include/ ||
        ( echo "cannot compile TinyXML" ; exit 1 )
popd

# libtommath
pushd libtommath
make clean &&
    make &&
    make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtommath/pdf ||
        ( echo "Cannot compile LTM" ; exit 1 )
popd

# libtomcrypt
pushd libtomcrypt
make clean &&
    CFLAGS="-I$output/include -DLTC_NO_ROLC -DLTM_DESC" make &&
    make install INSTALL_GROUP=`id -gn` INSTALL_USER=`id -un` LIBPATH=$output/lib INCPATH=$output/include DATAPATH=$output/share/doc/libtomcrypt/pdf NODOCS=1 ||
        ( echo "Cannot compile LTC" ; exit 1 )
popd

# wxwidgets
pushd wxwidgets
./configure --prefix=$output --enable-monolithic=yes --enable-shared=no --enable-unicode --enable-ffile \
                     --without-libtiff --enable-graphics_ctx $cfg_switches \
                     --disable-largefile CPPFLAGS="$CPPCFLAGS $WXWIDGETS_EXTRA_CFLAGS" &&
    make clean &&
    make &&
    make install ||
        ( echo "Cannot compile wxWidgets"; exit 1 )
popd

# wxwebkit
pushd webkit
make clean
PATH="$output/bin:${PATH}" ./WebKitTools/Scripts/build-webkit --wx --wx-args=wxgc,ENABLE_OFFLINE_WEB_APPLICATIONS=0,ENABLE_DOM_STORAGE=1,ENABLE_DATABASE=0,ENABLE_ICONDATABASE=0,ENABLE_XPATH=1,ENABLE_XSLT=1,ENABLE_VIDEO=0,ENABLE_SVG=0,ENABLE_COVERAGE=0,ENABLE_WML=0,ENABLE_WORKERS=0 &&
    mv ./WebKitBuild/Release/*.a $output/lib &&
    cp ./WebKit/wx/*.h $output/include/wx-2.8/wx ||
        ( echo "Cannot compile WebKit" ; exit 1 )
popd

