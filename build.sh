#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
glslc -fshader-stage=fragment src/triangle/fragment.glsl -o src/triangle/fragment.spv
glslc -fshader-stage=vertex src/triangle/vertex.glsl -o src/triangle/vertex.spv
cp "./build/$NAME" "./$NAME"
