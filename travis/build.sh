case "$TRAVIS_OS_NAME" in
    linux)
        mkdir -p _build/lib-x86-64/include
        mkdir -p _build/lib-x86-64/lib
        cd src
        ARCH=x86_64 ./build_all.sh
        cd ..
        echo --------------------
        echo Copying GTK libs
        echo --------------------
        cp -r libgtk-x86_64 _build/
        echo --------------------
        echo Packing
        echo --------------------
        tar jcvf ddb-static-deps-latest.tar.bz2 _build/lib-x86-64
    ;;
esac

