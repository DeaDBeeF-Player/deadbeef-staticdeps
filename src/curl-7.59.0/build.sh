./configure --enable-static --disable-ldap --disable-ldaps --with-ssl --with-zlib --without-gnutls --without-nss --without-libssh2 --without-librtmp --without-libidn --disable-nls --disable-rtsp --disable-verbose  $CONFIG_OPTS
make clean
make
