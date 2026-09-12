#!/bin/bash

NAME=sdl_gpu
SUBNAME=sprites

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/sprite.vertex.hlsl" -o "./src/$SUBNAME/compiled/sprite.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/sprite.fragment.hlsl" -o "./src/$SUBNAME/compiled/sprite.fragment.spv"
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/screen.vertex.hlsl" -o "./src/$SUBNAME/compiled/screen.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/$SUBNAME/screen.fragment.hlsl" -o "./src/$SUBNAME/compiled/screen.fragment.spv"
cp "./build/$NAME" "./$NAME"
