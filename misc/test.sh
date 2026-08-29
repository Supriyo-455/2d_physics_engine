#!/usr/bin/env bash
set -e

# ================================
# PROJECT DEFINES
# ================================
EXE_NAME="Tests"
code_path="../code/"
third_party_path="../third_party/"
source_assets="../assets/"

# ================================
# COMPILER FLAGS
# ================================
compiler="-std=c++20 -O0 -fno-rtti -fno-exceptions -Wall -Wextra -Werror -mrdseed"

# IGNORE WARNINGS
compiler="$compiler -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-missing-field-initializers -Wno-sign-compare"

# INCLUDE PATHS
compiler="$compiler -I${third_party_path}include"

# ================================
# DEBUG & DEFINES
# ================================
debug="-g3"
defines="-DENGINE_DEBUG=1"

# ================================
# LIBRARIES
# ================================
linux_libs="-lpthread -ldl -lGL"
sdl_libs="-lSDL2 -lSDL2_ttf -lSDL2_image"

# ================================
# LINKER
# ================================
link="-Wl,--gc-sections -L${third_party_path}lib"

# ================================
# BUILD DIR
# ================================
mkdir -p ./build
pushd ./build > /dev/null

rm -f *.o

# ================================
# BUILD EXECUTABLE
# ================================
g++ $compiler $defines $debug "${code_path}all_tests.cpp" \
    $link $sdl_libs $linux_libs \
    -o "$EXE_NAME"

popd > /dev/null

# ================================
# RUN TESTS
# ================================
./build/"$EXE_NAME"