################################################################
# Process this file with top-level configure script to produce Makefile
#
# Copyright 2000 Clark Cooper
#
#  This file is part of EXPAT.
#
#  EXPAT is free software; you can redistribute it and/or modify it
#  under the terms of the License (based on the MIT/X license) contained
#  in the file COPYING that comes with this distribution.
#
# EXPAT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN EXPAT.
#

SHELL = /bin/bash

srcdir = .
top_srcdir = .


prefix = /home/waker/prj/deadbeef-deps/lib-x86-64
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
man1dir = ${prefix}/man/man1

top_builddir = .


INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
mkinstalldirs = $(SHELL) $(top_srcdir)/conftools/mkinstalldirs

MANFILE = $(srcdir)/doc/xmlwf.1
APIHEADER = $(srcdir)/lib/expat.h $(srcdir)/lib/expat_external.h
LIBRARY = libexpat.la

DESTDIR = $(INSTALL_ROOT)

default:  buildlib xmlwf/xmlwf

buildlib: $(LIBRARY)

all: $(LIBRARY) xmlwf/xmlwf examples/elements examples/outline

clean:
	cd lib && rm -f $(LIBRARY) *.o *.lo && rm -rf .libs _libs
	cd xmlwf && rm -f xmlwf *.o *.lo && rm -rf .libs _libs
	cd examples && rm -f elements outline *.o *.lo && rm -rf .libs _libs
	cd tests && rm -rf .libs runtests runtests.o runtestspp runtestspp.o
	cd tests && rm -f chardata.o minicheck.o
	rm -rf .libs libexpat.la
	rm -f examples/core tests/core xmlwf/core

clobber: clean

distclean: clean
	rm -f expat_config.h config.status config.log config.cache libtool
	rm -f Makefile

extraclean: distclean
	rm -f expat_config.h.in configure
	rm -f conftools/ltconfig conftools/ltmain.sh conftools/libtool.m4

check: tests/runtests tests/runtestspp
	tests/runtests
	tests/runtestspp

install: xmlwf/xmlwf installlib
	$(mkinstalldirs) $(DESTDIR)$(bindir) $(DESTDIR)$(man1dir)
	$(LIBTOOL) --mode=install $(INSTALL_PROGRAM) xmlwf/xmlwf $(DESTDIR)$(bindir)/xmlwf
	$(INSTALL_DATA) $(MANFILE) $(DESTDIR)$(man1dir)

installlib: $(LIBRARY) $(APIHEADER)
	$(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(includedir)
	$(LIBTOOL) --mode=install $(INSTALL) $(LIBRARY) $(DESTDIR)$(libdir)/$(LIBRARY)
	for FN in $(APIHEADER) ; do $(INSTALL_DATA) $$FN $(DESTDIR)$(includedir) ; done

uninstall: uninstalllib
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(bindir)/xmlwf
	rm -f $(DESTDIR)$(man1dir)/xmlwf.1

uninstalllib:
	$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(libdir)/$(LIBRARY)
	rm -f $(DESTDIR)$(includedir)/expat.h
	rm -f $(DESTDIR)$(includedir)/expat_external.h

# for VPATH builds (invoked by configure)
mkdir-init:
	@for d in lib xmlwf examples tests ; do \
		(mkdir $$d 2> /dev/null || test 1) ; \
	done

CC = /home/waker/prj/deadbeef/tools/apbuild/apgcc
CXX = /home/waker/prj/deadbeef/tools/apbuild/apgcc
LIBTOOL = $(SHELL) $(top_builddir)/libtool

INCLUDES = -I$(srcdir)/lib -I.
LDFLAGS = -L/home/waker/prj/deadbeef-deps/lib-x86-64/lib
CPPFLAGS =  -DHAVE_EXPAT_CONFIG_H
CFLAGS = -O2 -D_GNU_SOURCE -D_FORTIFY_SOURCE=0 -fPIC -I/home/waker/prj/deadbeef-deps/lib-x86-64/include -Wall -Wmissing-prototypes -Wstrict-prototypes -fexceptions
CXXFLAGS = -O2 -D_GNU_SOURCE -D_FORTIFY_SOURCE=0 -fPIC -I/home/waker/prj/deadbeef-deps/lib-x86-64/include -Wall -fexceptions
VSNFLAG = -version-info 6:2:5

### autoconf this?
LTFLAGS = --silent

COMPILE = $(CC) $(INCLUDES) $(CFLAGS) $(DEFS) $(CPPFLAGS)
CXXCOMPILE = $(CXX) $(INCLUDES) $(CXXFLAGS) $(DEFS) $(CPPFLAGS)
LTCOMPILE = $(LIBTOOL) $(LTFLAGS) --mode=compile $(COMPILE)
LINK_LIB = $(LIBTOOL) $(LTFLAGS) --mode=link $(COMPILE) -no-undefined $(VSNFLAG) -rpath $(libdir) $(LDFLAGS) -o $@
LINK_EXE = $(LIBTOOL) $(LTFLAGS) --mode=link $(COMPILE) $(LDFLAGS) -o $@
LINK_CXX_EXE = $(LIBTOOL) $(LTFLAGS) --mode=link $(CXXCOMPILE) $(LDFLAGS) -o $@

LIB_OBJS = lib/xmlparse.lo lib/xmltok.lo lib/xmlrole.lo
$(LIBRARY): $(LIB_OBJS)
	$(LINK_LIB) $(LIB_OBJS)

lib/xmlparse.lo: lib/xmlparse.c lib/expat.h lib/xmlrole.h lib/xmltok.h \
	$(top_builddir)/expat_config.h lib/expat_external.h lib/internal.h

lib/xmlrole.lo: lib/xmlrole.c lib/ascii.h lib/xmlrole.h \
	$(top_builddir)/expat_config.h lib/expat_external.h lib/internal.h

lib/xmltok.lo: lib/xmltok.c lib/xmltok_impl.c lib/xmltok_ns.c \
	lib/ascii.h lib/asciitab.h lib/iasciitab.h lib/latin1tab.h \
	lib/nametab.h lib/utf8tab.h lib/xmltok.h lib/xmltok_impl.h \
	$(top_builddir)/expat_config.h lib/expat_external.h lib/internal.h


XMLWF_OBJS = xmlwf/xmlwf.o xmlwf/xmlfile.o xmlwf/codepage.o xmlwf/unixfilemap.o
xmlwf/xmlwf.o: xmlwf/xmlwf.c
xmlwf/xmlfile.o: xmlwf/xmlfile.c
xmlwf/codepage.o: xmlwf/codepage.c
xmlwf/unixfilemap.o: xmlwf/unixfilemap.c
xmlwf/xmlwf: $(XMLWF_OBJS) $(LIBRARY)
	$(LINK_EXE) $(XMLWF_OBJS) $(LIBRARY)

examples/elements.o: examples/elements.c
examples/elements: examples/elements.o $(LIBRARY)
	$(LINK_EXE) $< $(LIBRARY)

examples/outline.o: examples/outline.c
examples/outline: examples/outline.o $(LIBRARY)
	$(LINK_EXE) $< $(LIBRARY)

tests/chardata.o: tests/chardata.c tests/chardata.h
tests/minicheck.o: tests/minicheck.c tests/minicheck.h
tests/runtests.o: tests/runtests.c tests/chardata.h
tests/runtests: tests/runtests.o tests/chardata.o tests/minicheck.o $(LIBRARY)
	$(LINK_EXE) tests/runtests.o tests/chardata.o tests/minicheck.o $(LIBRARY)
tests/runtestspp.o: tests/runtestspp.cpp tests/runtests.c tests/chardata.h
tests/runtestspp: tests/runtestspp.o tests/chardata.o tests/minicheck.o $(LIBRARY)
	$(LINK_CXX_EXE) tests/runtestspp.o tests/chardata.o tests/minicheck.o $(LIBRARY)

tests/xmlts.zip:
	wget --output-document=tests/xmlts.zip \
		http://www.w3.org/XML/Test/xmlts20020606.zip

tests/XML-Test-Suite: tests/xmlts.zip
	cd tests && unzip -q xmlts.zip

run-xmltest: xmlwf/xmlwf tests/XML-Test-Suite
	tests/xmltest.sh

.SUFFIXES: .c .cpp .lo .o

.cpp.o:
	$(CXXCOMPILE) -o $@ -c $<
.c.o:
	$(COMPILE) -o $@ -c $<
.c.lo:
	$(LTCOMPILE) -o $@ -c $<

.PHONY: buildlib all \
	clean distclean extraclean maintainer-clean \
	dist distdir \
	install uninstall
