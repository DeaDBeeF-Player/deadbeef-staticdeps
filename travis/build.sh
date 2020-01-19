case "$TRAVIS_OS_NAME" in
    linux)
        mkdir -p _build/lib-x86-64/include
        mkdir -p _build/lib-x86-64/lib
        cd src
        ARCH=x86_64 ./build_all.sh
        cd ..
    ;;
esac

