ORIGIN=`pwd`
CFLAGS="$CFLAGS -I${ORIGIN}/include"
./configure --enable-static $CONFIG_OPTS
make clean || exit 1
make || exit 1
cp common/mp4ff/libmp4ff.a $PREFIX/lib/
#cp common/mp4ff/mp4ff.h $PREFIX/include/
