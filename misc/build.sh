#!/usr/bin/env bash
set -e

# ================================
# PROJECT DEFINES
# ================================
EXE_NAME="Game"
code_path="../code/"
third_party_path="../third_party/"
source_assets="../assets/"

# ================================
# COMPILER FLAGS
# ================================
# Equivalent MSVC flags:
# -std:c++20 -> -std=c++20
# -Od        -> -O0
# -GR-       -> -fno-rtti
# -EHa-      -> -fno-exceptions
# -W4 -WX    -> -Wall -Wextra -Werror
compiler="-std=c++20 -O0 -fno-rtti -fno-exceptions -Wall -Wextra -Werror -mrdseed"

# IGNORE WARNINGS
# (Equivalent to -wd4201, -wd4100, -wd4189, etc.)
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
# Replaced Windows APIs with POSIX equivalents (-ldl, -lpthread, -lGL)
linux_libs="-lpthread -ldl -lGL"
sdl_libs="-lSDL2 -lSDL2_ttf -lSDL2_image"

# ================================
# LINKER FLAGS
# ================================
link="-Wl,--gc-sections -L${third_party_path}lib"

# ================================
# BUILD DIR
# ================================
mkdir -p ./build
pushd ./build > /dev/null

# Copy shared libraries (.so instead of .dll)
if [ -d "${third_party_path}lib" ]; then
    cp -u "${third_party_path}lib"/*.so . 2>/dev/null || true
fi

# Copy assets
if [ -d "${source_assets}" ]; then
    mkdir -p ./assets
    cp -ru "${source_assets}"* ./assets/ 2>/dev/null || true
fi

# Clean previous build artifacts
rm -f *.o

# ================================
# BUILD EXECUTABLE
# ================================
g++ $compiler $defines $debug "${code_path}game.cpp" \
    $link $sdl_libs $linux_libs \
    -o "$EXE_NAME"

popd > /dev/null