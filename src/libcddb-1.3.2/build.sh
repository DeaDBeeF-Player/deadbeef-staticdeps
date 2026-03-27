./configure --enable-static --disable-rpath $CONFIG_OPTS
make clean
make CFLAGS="$CFLAGS -Wno-error=incompatible-pointer-types"

