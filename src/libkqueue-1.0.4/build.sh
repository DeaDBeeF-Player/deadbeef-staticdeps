#!/bin/bash
./configure --disable-static --enable-shared $CONFIG_OPTS || exit 1
make || exit 1

