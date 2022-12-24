aclocal
libtoolize --force
automake --add-missing
autoreconf
./configure --enable-static --disable-shared $CONFIG_OPTS
make clean
make -j8
