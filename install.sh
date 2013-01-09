#!/bin/sh

phpize5
./configure
export LIBS=/usr/local/lib/libopencv_*.so.*
make && make install

