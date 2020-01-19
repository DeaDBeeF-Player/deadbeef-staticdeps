#!/bin/sh

mkdir -p lib-x86-64/gtk-3.10.8
cd lib-x86-64/gtk-3.10.8
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gtk+3.0/libgtk-3-0_3.10.8-0ubuntu1.4_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/atk1.0/libatk1.0-0_2.10.0-2ubuntu2_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo2_1.13.0~20140204-0ubuntu1_amd64.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gdk-pixbuf/libgdk-pixbuf2.0-0_2.30.7-0ubuntu1.6_amd64.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/libp/libpng/libpng12-0_1.2.50-1ubuntu2.14.04.2_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/g/glib2.0/libglib2.0-0_2.40.0-2_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpango-1.0-0_1.36.3-1ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangocairo-1.0-0_1.36.3-1ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangoft2-1.0-0_1.36.3-1ubuntu1_amd64.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/f/freetype/libfreetype6_2.5.2-1ubuntu2.8_amd64.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gtk+3.0/libgtk-3-dev_3.10.8-0ubuntu1.4_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/atk1.0/libatk1.0-dev_2.10.0-2ubuntu2_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/g/glib2.0/libglib2.0-dev_2.40.0-2_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo2-dev_1.13.0~20140204-0ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/at-spi2-atk/libatk-bridge2.0-dev_2.10.2-2ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpango1.0-dev_1.36.3-1ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo-script-interpreter2_1.13.0~20140204-0ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/at-spi2-atk/libatk-bridge2.0-0_2.10.2-2ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo-gobject2_1.13.0~20140204-0ubuntu1_amd64.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangoxft-1.0-0_1.36.3-1ubuntu1_amd64.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gdk-pixbuf/libgdk-pixbuf2.0-dev_2.30.7-0ubuntu1.6_amd64.deb

for i in *.deb ; do
	ar x $i
	tar xxvf data.tar.xz
	rm data.tar.xz
	rm debian-binary
	rm control.tar.gz
done

cd ../..
mkdir -p lib-x86-32/gtk-3.10.8
cd lib-x86-32/gtk-3.10.8
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gtk+3.0/libgtk-3-0_3.10.8-0ubuntu1.4_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/atk1.0/libatk1.0-0_2.10.0-2ubuntu2_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo2_1.13.0~20140204-0ubuntu1_i386.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gdk-pixbuf/libgdk-pixbuf2.0-0_2.30.7-0ubuntu1.6_i386.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/libp/libpng/libpng12-0_1.2.50-1ubuntu2.14.04.2_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/g/glib2.0/libglib2.0-0_2.40.0-2_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpango-1.0-0_1.36.3-1ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangocairo-1.0-0_1.36.3-1ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangoft2-1.0-0_1.36.3-1ubuntu1_i386.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/f/freetype/libfreetype6_2.5.2-1ubuntu2.8_i386.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gtk+3.0/libgtk-3-dev_3.10.8-0ubuntu1.4_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/atk1.0/libatk1.0-dev_2.10.0-2ubuntu2_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/g/glib2.0/libglib2.0-dev_2.40.0-2_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo2-dev_1.13.0~20140204-0ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/at-spi2-atk/libatk-bridge2.0-dev_2.10.2-2ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpango1.0-dev_1.36.3-1ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo-script-interpreter2_1.13.0~20140204-0ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/a/at-spi2-atk/libatk-bridge2.0-0_2.10.2-2ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/c/cairo/libcairo-gobject2_1.13.0~20140204-0ubuntu1_i386.deb
wget -c http://mirrors.kernel.org/ubuntu/pool/main/p/pango1.0/libpangoxft-1.0-0_1.36.3-1ubuntu1_i386.deb
wget -c http://security.ubuntu.com/ubuntu/pool/main/g/gdk-pixbuf/libgdk-pixbuf2.0-dev_2.30.7-0ubuntu1.6_i386.deb

for i in *.deb ; do
	ar x $i
	tar xxvf data.tar.xz
	rm data.tar.xz
	rm debian-binary
	rm control.tar.gz
done

