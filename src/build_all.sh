#!/bin/bash

ORIGIN=`pwd`
OUTPUT=$ORIGIN/../_build
AP=$ORIGIN/../External/apbuild

export CC=$AP/apgcc
export CXX=$AP/apg++
export APBUILD_STATIC_LIBGCC=1
export MAKEFLAGS="-j8"
cd $AP
./apinit || exit 1
cd $ORIGIN

#export ARCH=`uname -m | perl -ne 'chomp and print'`

if [[ "$ARCH" == "i686" ]]; then
    export PREFIX=$OUTPUT/lib-x86-32
    export CFLAGS="-m32 -fPIC -I$PREFIX/include/i386-linux-gnu"
    export LDFLAGS='-m32'
    export PKG_CONFIG_PATH=$ORIGIN/lib-x86-32/lib/pkg-config
    export LD_LIBRARY_PATH=$ORIGIN/lib-x86-32/lib
    export CHOST="i686-unknown-linux-gnu"
    export CONFIG_OPTS="--prefix=$PREFIX --build=i686-unknown-linux-gnu"
elif [[ "$ARCH" == "x86_64" ]]; then
    DDB_ARCHNAME='x86-64'
    TOOLCHAIN_ARCH='x86_64-linux-gnu'
    LIBPATH="lib-$DDB_ARCHNAME"
    export CHOST="x86_64-unknown-linux-gnu"
    export PREFIX=$OUTPUT/$LIBPATH
    GTK_ROOT_216="$ORIGIN/../_build/$LIBPATH/gtk-2.16.0/"
    GTK_ROOT_310="$ORIGIN/../_build/$LIBPATH/gtk-3.10.8/"
    export GLIB_CFLAGS="-I${GTK_ROOT_310}/include/gio-unix-2.0/ -I${GTK_ROOT_310}/include/glib-2.0 -I${GTK_ROOT_310}/lib/glib-2.0/include -I${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH -I${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH/glib-2.0/include";
    export GLIB_LIBS="-L${GTK_ROOT_310}/lib -L${GTK_ROOT_310}/lib -L${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH -lgobject-2.0 -lgthread-2.0 -lglib-2.0 -lgio-2.0";
    export CFLAGS='-m64 -fPIC'
    export LDFLAGS='-m64'
    export PKG_CONFIG_PATH=$ORIGIN/$LIBPATH/lib/pkg-config
    export LD_LIBRARY_PATH=$ORIGIN/$LIBPATH/lib
    export CONFIG_OPTS="--prefix=$PREFIX --build=$CHOST"
elif [[ "$ARCH" == "aarch64" ]]; then
    DDB_ARCHNAME='arm64'
    TOOLCHAIN_ARCH='aarch64-linux-gnu'
    LIBPATH="lib-$DDB_ARCHNAME"
    export CHOST="aarch64-unknown-linux-gnu"
    export PREFIX=$OUTPUT/$LIBPATH
    GTK_ROOT_216="$ORIGIN/../_build/$LIBPATH/gtk-2.16.0/"
    GTK_ROOT_310="$ORIGIN/../_build/$LIBPATH/gtk-3.10.8/"
    export GLIB_CFLAGS="-I${GTK_ROOT_310}/include/gio-unix-2.0/ -I${GTK_ROOT_310}/include/glib-2.0 -I${GTK_ROOT_310}/lib/glib-2.0/include -I${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH -I${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH/glib-2.0/include";
    export GLIB_LIBS="-L${GTK_ROOT_310}/lib -L${GTK_ROOT_310}/lib -L${GTK_ROOT_310}/lib/$TOOLCHAIN_ARCH -lgobject-2.0 -lgthread-2.0 -lglib-2.0 -lgio-2.0";
    export CFLAGS='-m64 -fPIC'
    export LDFLAGS='-m64'
    export PKG_CONFIG_PATH=$ORIGIN/$LIBPATH/lib/pkg-config
    export LD_LIBRARY_PATH=$ORIGIN/$LIBPATH/lib
    export CONFIG_OPTS="--prefix=$PREFIX --build=$CHOST"
else
    echo unknown arch $ARCH
    exit 1
fi
export INCLUDE=$PREFIX/include
export LIB=$PREFIX/lib
export CFLAGS="$CFLAGS -O2 -D_GNU_SOURCE -D_FORTIFY_SOURCE=0 -fPIC -I$INCLUDE"
export CXXFLAGS=$CFLAGS
export LDFLAGS="$LDFLAGS -L$LIB"

echo "Configure options: $CONFIG_OPTS"
export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig

# quirks
#   json-glib required host glib binaries for glib-mkenums and probably more
#       because of that, host tools are used, but headers/libs from the target
#   libzip is using cmake, and toolchain configuration works "by accident"
#   libdbus requires expat.h in the host include path - may cause undefined behavior
#   libsndfile requires autogen to be installed for whatever reason
#   libsndfile configure.ac is patched to prevent building example programs
#   libogg requires autoreconf to be run
#   libopusfile fails to build examples, needs patching
#   libidn required Makefile.in patching to remove docs, examples, etc
#   libmp4ff.a is noinst, need to be copied manually

libs="swift-corelibs-libdispatch-swift-5.5-RELEASE libsndfile-1.0.28 libcdio-2.1.0 libcdio-paranoia-10.2+2.0.1 alsa-lib-1.0.25 mbedtls-2.7.19 curl-7.79.1 json-glib-0.14.2 expat-2.0.1 dbus-1.4.0 dbus-glib-0.100 zlib-1.2.5 libzip-1.5.2 jpeg-8c libmad-0.15.1b libxml2-2.7.8 mpg123-1.22.4 libogg-1.3.2 libvorbis-1.3.4 flac-1.5.0 libpng-1.5.2 libsamplerate-0.1.9 opus-1.1 opusfile-0.6 sqlite-autoconf-3080301 libcddb-1.3.2 opencore-amr-0.1.2 ffmpeg-4.4 jansson-2.12 fftw-3.3.8 faad2-2.8.8 wavpack-5.1.0"

mkdir -p $PREFIX
for i in $libs ; do

    cd "$i"
    echo -----------------
    echo Building $i
    echo -----------------
    echo $ARCH
    echo CFLAGS=$CFLAGS
    echo CXXFLAGS=$CFLAGS
    echo LDFLAGS=$LDFLAGS
    sh ./build.sh || exit 1
    echo installing $i to $PREFIX
    if [[ "$i" =~ "zlib-" ]] ; then
        cp zlib.h $PREFIX/include/
        cp zconf.h $PREFIX/include/
        cp libz.a $PREFIX/lib/
    else 
        echo
        make install || exit 1
    fi

    cd ..
    echo -----------------
    echo Finished building $i
    echo -----------------

done

mkdir -p $PREFIX/lib
mkdir -p $PREFIX/include

# libkqueue -- still using gcc, since clang is too pedantic with warnings
#echo "---- building libkqueue"
#cd libkqueue-1.0.4
## need to avoid custom cflags, to prevent _GNU_SOURCE redefinition warning
#CFLAGS="-m64 -O2 -D_FORTIFY_SOURCE=0 -fPIC -I$INCLUDE" sh ./build.sh || exit 1
#ln -s libkqueue.so.0.0 libkqueue.so.0
#cp -P *.so* $PREFIX/lib/ || exit 1
#cp -r include $PREFIX/ || exit 1
#cd ..

# clang builds
#export APGCC_USE_CLANG=1

# libBlocksRuntime
#echo "---- building libBlocksRuntime"
#cd libBlocksRuntime-0.1
#sh ./build.sh || exit 1
## add missing symlink
#ln -s libBlocksRuntime.so.0.0 libBlocksRuntime.so.0
#cp -P *.so* $PREFIX/lib/ || exit 1
#cp Block*.h $PREFIX/include/ || exit 1
#cd ..

# libdispatch
#echo "---- building libdispatch"
#cd libdispatch-0~svn197
#
#KQUEUE_CFLAGS="-I$PREFIX/include" KQUEUE_LIBS="-L$PREFIX/lib -lkqueue" sh ./build.sh || exit 1
#cp -P src/.libs/libdispatch.so* $PREFIX/lib/ || exit 1
#mkdir -p $PREFIX/include/dispatch || exit 1
#cp dispatch/*.h $PREFIX/include/dispatch/ || exit 1
#cd ..

echo -----------------
echo Cleaning up the build artifacts
echo -----------------
rm -rf $PREFIX/lib/libzip 2>&1 >/dev/null
rm -rf $PREFIX/bin 2>&1 >/dev/null
rm -rf $PREFIX/etc 2>&1 >/dev/null
rm -rf $PREFIX/libexec 2>&1 >/dev/null
rm -rf $PREFIX/man 2>&1 >/dev/null
rm -rf $PREFIX/share 2>&1 >/dev/null
rm $PREFIX/lib/*.la 2>&1 >/dev/null

# delete unwanted shared libs
find $PREFIX/lib -type f -name "*.so*" ! -name "*asound*" ! -name "*dispatch*" ! -name "*Blocks*" ! -name "*kqueue*" ! -name "*libcurl*" ! -name "*libmbed*" ! -name "*libcddb*" ! -name "*sndfile*" ! -name "*faad*" ! -name "*opencore*" ! -name "*libz*" ! -name "*libzip*" ! -name "*libav*" ! -name "*libopus*" ! -name "*dbus*" ! -name "*libexpat*" ! -name "*libmad*" ! -name "*libmpg123" ! -name "*wavpack*" ! -name "*samplerate*" -exec rm {} +
