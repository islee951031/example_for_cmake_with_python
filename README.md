# example_for_cmake_with_python
This repository contains a simple project for making a executable file which is built with a c++ compiler and runs python scripts.

This repository is a reference material for constructing the projects written with c++ and python.

# Pre-requirement
cmake (above 3.18)

python (3.12.10)

mingw64 (x86_64-posix-seh-rev0)

Vulkan SDK


# Build Method 
mkdir build

cd build 

cmake .. -G "MinGW Makefiles"

cmake --build .
