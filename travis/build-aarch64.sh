case "$TRAVIS_OS_NAME" in
    linux)
        mkdir -p _build/lib-aarch64/include || exit 1
        mkdir -p _build/lib-aarch64/lib || exit 1

        echo --------------------
        echo Copying GTK libs
        echo --------------------
#        cp -r libgtk-aarch64/gtk-2.16.0 _build/lib-aarch64/ || exit 1
        cp -r libgtk-aarch64/gtk-3.24.50 _build/lib-aarch64/ || exit 1

        cd src || exit 1
        ARCH=aarch64 ./build_all.sh || exit 1
        cd .. || exit 1

        echo --------------------
        echo Copying prebuilt libs
        echo --------------------
        cp -r prebuilt/aarch64/include _build/lib-aarch64/ || exit 1
        cp -r prebuilt/aarch64/lib _build/lib-aarch64/ || exit 1

        echo --------------------
        echo Packing
        echo --------------------
        cd _build
        tar jcvf ddb-static-deps-aarch64-latest.tar.bz2 lib-aarch64 || exit 1
        cd ..
    ;;
esac

