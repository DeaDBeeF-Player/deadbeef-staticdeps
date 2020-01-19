CC?=gcc
AR?=ar
CFLAGS+=-DSQLITE_ENABLE_FTS3 -DSQLITE_ENABLE_RTREE
PREFIX?=/usr/local

all:
	$(CC) $(CFLAGS) -c sqlite3.c -o sqlite3.o
	$(AR) rcs libsqlite3.a sqlite3.o

install:
	cp libsqlite3.a $(PREFIX)/lib
	cp sqlite3.h $(PREFIX)/include
	
