#!/bin/bash
rm -rf ./build
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=DEBUG  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON  -DTEST_MODE_NO_WIRINGPI_LINK=ON ../
make 

