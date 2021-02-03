autoreconf -f -i
./configure --disable-shared --enable-static --with-ogg-includes=../libogg-1.2.0/include --disable-cpplibs --disable-xmms-plugin --disable-doxygen-docs --disable-ogg $CONFIG_OPTS
make clean
make

