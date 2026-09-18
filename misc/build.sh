#!/usr/bin/env bash
set -e

EXE_NAME="Game"
CODE_PATH="$PWD/code/"
LIBS_PATH="$PWD/third_party/"
ASSETS_PATH="$PWD/assets/"

# Extract project name from 4coder config, fallback to a default if missing
PROJECT_NAME=$(awk -F'"' '/project_name/ {print $2}' project.4coder 2>/dev/null)
PROJECT_NAME="${PROJECT_NAME:-2d_phyics}"

# COMPILER FLAGS
CXXFLAGS=(
    "-std=c++20"                      # Target the C++20 standard
    "-O0"                             # Disable optimization for faster builds and better debugging
    "-fno-diagnostics-show-caret"     # Force single-line errors so 4coder can parse them natively
    "-fno-rtti"                       # Disable Run-Time Type Info (reduces overhead if not using dynamic_cast)
    "-fno-exceptions"                 # Disable C++ exceptions to enforce explicit error handling
	"-fno-show-column"                # Strips column output for strict 4coder parsing
    "-Wall"                           # Enable standard compiler warnings
    "-Wextra"                         # Enable extended compiler warnings
    "-Werror"                         # Treat warnings as errors to enforce clean code
    "-mrdseed"                        # Enable x86 RDSEED hardware instructions for random number generation
    "-Wno-unused-parameter"           # Suppress warnings for unused function arguments
    "-Wno-unused-variable"            # Suppress warnings for unused local variables
    "-Wno-unused-function"            # Suppress warnings for unused internal functions
    "-Wno-missing-field-initializers" # Suppress warnings for partial struct initialization
    "-Wno-sign-compare"               # Suppress warnings when comparing signed and unsigned integers
    "-I${LIBS_PATH}include"           # Add third-party include directory to search path
    "-g3"                             # Generate maximum debug information for GDB
    "-DENGINE_DEBUG=1"                # Define the ENGINE_DEBUG preprocessor macro
)

# LINKER FLAGS
LDFLAGS=(
    "-Wl,--gc-sections"               # Instruct linker to strip out unused code and data sections
    "-L${LIBS_PATH}lib"               # Add third-party library directory to search path
    "-lpthread"                       # Link POSIX threading library
    "-ldl"                            # Link dynamic loader (required for dlopen/dlsym)
    "-lGL"                            # Link OpenGL library
    "-lSDL2"                          # Link core SDL2 library
    "-lSDL2_ttf"                      # Link SDL2 TrueType font extension
    "-lSDL2_image"                    # Link SDL2 image loading extension
)

mkdir -p build/assets

# Copy files directly into build/ without changing the working directory
cp -u "${LIBS_PATH}lib"/*.so build/ 2>/dev/null || true
cp -ru "${ASSETS_PATH}"* build/assets/ 2>/dev/null || true
rm -f build/*.o

# Building the executable
g++ "${CXXFLAGS[@]}" "${CODE_PATH}game.cpp" "${LDFLAGS[@]}" -o "build/$EXE_NAME"


# GENERATE QT CREATOR PROJECT FILES
touch "${PROJECT_NAME}.creator"

find "$CODE_PATH" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) > "${PROJECT_NAME}.files"

echo "${CODE_PATH}" > "${PROJECT_NAME}.includes"
echo "${LIBS_PATH}include" >> "${PROJECT_NAME}.includes"

printf "%s\n" "${CXXFLAGS[@]}" > "${PROJECT_NAME}.cxxflags"

mkdir -p .qtcreator
if [ ! -f ".qtcreator/${PROJECT_NAME}.creator.user" ] && [ -f "debug_template.user.backup" ]; then
    cp debug_template.user.backup ".qtcreator/${PROJECT_NAME}.creator.user"
fi