#!/bin/sh
phpize5
export LIBS=/usr/local/lib/libopencv_*.so.*
./configure
make && sudo make install