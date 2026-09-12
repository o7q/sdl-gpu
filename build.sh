#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/sprite.vertex.hlsl" -o "./src/compiled/sprite.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/sprite.fragment.hlsl" -o "./src/compiled/sprite.fragment.spv"
"./SDL_shadercross-main/build/shadercross" "./src/screen.vertex.hlsl" -o "./src/compiled/screen.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/screen.fragment.hlsl" -o "./src/compiled/screen.fragment.spv"
cp "./build/$NAME" "./$NAME"
