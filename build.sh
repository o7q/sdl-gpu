#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
./SDL_shadercross-main/build//shadercross ./src/triangle/shader.vertex.hlsl -o ./src/triangle/vertex.spv
./SDL_shadercross-main/build//shadercross ./src/triangle/shader.fragment.hlsl -o ./src/triangle/fragment.spv
cp "./build/$NAME" "./$NAME"
