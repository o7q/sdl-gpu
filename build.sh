#!/bin/bash

NAME=sdl_gpu
SUBNAME=triangles

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/shader.vertex.hlsl" -o "./src/$SUBNAME/vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/shader.fragment.hlsl" -o "./src/$SUBNAME/fragment.spv"
cp "./build/$NAME" "./$NAME"
