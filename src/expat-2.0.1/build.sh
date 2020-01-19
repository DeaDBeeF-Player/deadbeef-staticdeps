#!/bin/sh
./configure --enable-static $CONFIG_OPTS
make clean
make
