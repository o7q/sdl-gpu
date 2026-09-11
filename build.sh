#!/bin/bash

NAME=sdl_gpu
SUBNAME=shaderblend

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/quad.vertex.hlsl" -o "./src/$SUBNAME/quad.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/quad.fragment.hlsl" -o "./src/$SUBNAME/quad.fragment.spv"
cp "./build/$NAME" "./$NAME"
