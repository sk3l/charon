#!/bin/bash

rm -rf build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$(readlink -f .) ../src
