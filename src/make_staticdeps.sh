rm lib-x86-32/gtk-3.10.8/*.deb
rm -rf lib-x86-32/gtk-3.10.8/etc
rm -rf lib-x86-32/gtk-3.10.8/usr/bin
rm -rf lib-x86-32/gtk-3.10.8/usr/share
rm lib-x86-64/gtk-3.10.8/*.deb
rm -rf lib-x86-64/gtk-3.10.8/etc
rm -rf lib-x86-64/gtk-3.10.8/usr/bin
rm -rf lib-x86-64/gtk-3.10.8/usr/share
tar jcvf ddb-static-deps-latest.tar.bz2 lib-x86-32 lib-x86-64
