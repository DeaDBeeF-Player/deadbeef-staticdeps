ORIGIN=`pwd`
CFLAGS="$CFLAGS -I${ORIGIN}/include"
./configure --enable-static $CONFIG_OPTS
make clean
make

