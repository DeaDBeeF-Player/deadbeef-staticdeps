./configure --enable-static $CONFIG_OPTS
make clean
make CFLAGS=-fPIC CC=$CC CXX=$CXX -j8
