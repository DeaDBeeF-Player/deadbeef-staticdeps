## Process this file with automake to produce Makefile.in
# Copyright (C) 2002, 2003, 2004, 2006, 2007, 2008, 2009, 2010 Simon Josefsson
#
# This file is part of GNU Libidn.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = -I$(top_srcdir)/lib -I../lib -I$(top_srcdir)/gl -I../gl
AM_LDFLAGS = -static

LDADD = libutils.a ../lib/libidn.la

check_LIBRARIES = libutils.a
libutils_a_SOURCES = utils.h utils.c

ctests = tst_stringprep tst_punycode tst_idna tst_idna2 tst_nfkc	\
	tst_pr29 tst_strerror tst_toutf8 tst_symbols
if TLD
ctests += tst_tld
endif

TESTS = $(ctests)	
check_PROGRAMS = $(ctests)

TESTS_ENVIRONMENT = $(VALGRIND)
