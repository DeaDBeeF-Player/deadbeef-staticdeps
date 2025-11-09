autoreconf -f -i
./configure --disable-shared --enable-static --with-ogg-includes=../libogg-1.3.2/include --disable-cpplibs --disable-xmms-plugin --disable-doxygen-docs $CONFIG_OPTS
make clean
make

