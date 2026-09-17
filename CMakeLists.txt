cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(sdl_gpu)

file(GLOB_RECURSE SOURCES "src/*.cpp" "../px/src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "./src")
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "../px/include")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# target_compile_options(${PROJECT_NAME} PRIVATE -Og -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wall -Wpedantic -Wshadow -Wuninitialized)
# target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
target_compile_options(${PROJECT_NAME} PRIVATE -O2 -Wall -Wextra -Wpedantic -Wshadow -Wuninitialized -Wconversion -Wsign-conversion -Wnon-virtual-dtor -Wnull-dereference -Wdouble-promotion)
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)

find_package(SDL3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
