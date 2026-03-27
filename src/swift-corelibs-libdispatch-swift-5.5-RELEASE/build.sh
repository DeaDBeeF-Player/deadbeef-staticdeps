#!/bin/bash
CFLAGS="-Wno-error=cast-function-type-mismatch -Wno-error=unused-but-set-variable" \
cmake -DCMAKE_INSTALL_PREFIX:PATH=$PREFIX -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++  .
make clean
make VERBOSE=1
