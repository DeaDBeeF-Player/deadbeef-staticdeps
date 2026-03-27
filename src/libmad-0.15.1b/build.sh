make distclean

if [[ "$ARCH" == "i686" ]]; then
    FPM=intel
elif [[ "$ARCH" == "x86_64" ]]; then
    FPM=intel
elif [[ "$ARCH" == "aarch64" ]]; then
    FPM=arm
else
    FPM=default
fi

./configure --enable-static --disable-shared --enable-speed --enable-fpm=$FPM $CONFIG_OPTS
make clean
make CFLAGS="-O2 -fomit-frame-pointer -fPIC"
