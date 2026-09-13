#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/draw.vertex.hlsl" -o "./src/compiled/draw.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/draw.fragment.hlsl" -o "./src/compiled/draw.fragment.spv"
"./SDL_shadercross-main/build/shadercross" "./src/blit.vertex.hlsl" -o "./src/compiled/blit.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/blit.fragment.hlsl" -o "./src/compiled/blit.fragment.spv"
cp "./build/$NAME" "./$NAME"
