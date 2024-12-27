cmake_minimum_required(VERSION 3.23)

project(koby-lang)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

include_directories(src/core)

add_executable(koby ${SOURCE_FILES})