case "$TRAVIS_OS_NAME" in
    linux)
        mkdir -p _build/lib-x86-64/include || exit 1
        mkdir -p _build/lib-x86-64/lib || exit 1

        echo --------------------
        echo Copying GTK libs
        echo --------------------
        cp -r libgtk-x86_64/gtk-2.16.0 _build/lib-x86-64/ || exit 1
        cp -r libgtk-x86_64/gtk-3.10.8 _build/lib-x86-64/ || exit 1

        cd src || exit 1
        ARCH=x86_64 ./build_all.sh || exit 1
        cd .. || exit 1
        echo --------------------
        echo Packing
        echo --------------------
        tar jcvf _build/ddb-static-deps-latest.tar.bz2 _build/lib-x86-64 || exit 1
    ;;
esac

