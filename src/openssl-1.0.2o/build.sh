#!/bin/sh
./Configure linux-elf -m32
make clean
make -j8
