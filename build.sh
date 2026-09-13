#!/bin/bash

NAME=sdl_gpu

mkdir "build"
cmake -S . -B build -G Ninja -DCMAKE_COLOR_DIAGNOSTICS=ON
cmake --build build -j8
"./SDL_shadercross-main/build/shadercross" "./src/renderer/draw.vertex.hlsl" -o "./src/renderer/compiled/draw.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/renderer/draw.fragment.hlsl" -o "./src/renderer/compiled/draw.fragment.spv"
"./SDL_shadercross-main/build/shadercross" "./src/renderer/blit.vertex.hlsl" -o "./src/renderer/compiled/blit.vertex.spv"
"./SDL_shadercross-main/build/shadercross" "./src/renderer/blit.fragment.hlsl" -o "./src/renderer/compiled/blit.fragment.spv"
cp "./build/$NAME" "./$NAME"
