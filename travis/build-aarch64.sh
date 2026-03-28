#!/bin/bash

set -e

case "$TRAVIS_OS_NAME" in
    linux)
        mkdir -p _build/lib-aarch64/include
        mkdir -p _build/lib-aarch64/lib

        echo --------------------
        echo -e "\033[0;32mPatching libsubc++\033[37;0m"
        echo --------------------
        mkdir temp
        cp prebuilt/aarch64/lib/gcc/aarch64-linux-gnu/4.7/libsupc++.a temp/
        cd temp
        ar x libsupc++.a
        for i in *.o ; do mv $i new_$i ; objcopy new_$i $i --redefine-sym memcpy=memcpy@GLIBC_2.2.5 ; rm new_$i ; done
        ar rcs libsupc++.a *.o
        cd ..
        cp temp/libsupc++.a prebuilt/aarch64/lib/gcc/aarch64-linux-gnu/4.7/
        rm -rf temp
        echo -e "\033[0;32mDONE patching libsupc++\033[37;0m"

        echo --------------------
        echo -e "\033[0;32mCopying GTK libs\033[37;0m"
        echo --------------------
#        cp -r libgtk-aarch64/gtk-2.16.0 _build/lib-aarch64/
        cp -r libgtk-aarch64/gtk-3.10.8 _build/lib-aarch64/


        echo --------------------
        echo -e "\033[0;32mbuild_all.sh\033[37;0m"
        echo --------------------
        cd src
        ARCH=aarch64 ./build_all.sh
        cd ..

        echo --------------------
        echo -e "\033[0;32mCopying prebuilt libs\033[37;0m"
        echo --------------------
        cp -r prebuilt/aarch64/include _build/lib-aarch64/
        cp -r prebuilt/aarch64/lib _build/lib-aarch64/



        echo --------------------
        echo -e "\033[0;32mPacking\033[37;0m"
        echo --------------------
        cd _build
        tar jcvf ddb-static-deps-aarch64-latest.tar.bz2 lib-aarch64
        cd ..

        echo -e "\033[0;32mSTATIC DEPS BUILD COMPLETE SUCCESSFULLY\033[37;0m"
    ;;
esac

