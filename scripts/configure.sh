#!/bin/bash

#CC=clang CXX=clang++"" cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
CC=clang CXX=clang++"" cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
