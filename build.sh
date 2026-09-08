#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
cp "./build/$NAME" "./$NAME"
