#!/bin/bash

NAME=sdl_gpu
OUTPUT=./build_windows

mkdir "./$OUTPUT"
cmake -B "./$OUTPUT" -G Ninja -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build "./$OUTPUT" -j8
cp "./$OUTPUT/$NAME.exe" "./$NAME.exe"
