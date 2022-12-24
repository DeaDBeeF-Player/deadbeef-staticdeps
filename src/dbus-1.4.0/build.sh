./configure --with-xml=expat --enable-static --without-x --disable-inotify --localstatedir=/var --with-systemdsystemunitdir=/tmp/systemd $CONFIG_OPTS
make clean
make CFLAGS=-fPIC -j8
