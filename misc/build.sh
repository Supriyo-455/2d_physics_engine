#!/usr/bin/env bash
set -e

EXE_NAME="Game"
DLL_NAME="game.so"
CODE_PATH="$PWD/code/"
LIBS_PATH="$PWD/third_party/"
ASSETS_PATH="$PWD/assets/"

PROJECT_NAME=$(awk -F'"' '/project_name/ {print $2}' project.4coder 2>/dev/null)
PROJECT_NAME="${PROJECT_NAME:-2d_phyics}"

# COMPILER FLAGS
CXXFLAGS=(
    "-std=c++20"
    "-O0"
    "-fno-diagnostics-show-caret"
    "-fno-rtti"
    "-fno-exceptions"
    "-fno-show-column"
    "-Wall"
    "-Wextra"
    "-Werror"
    "-mrdseed"
    "-Wno-unused-parameter"
    "-Wno-unused-variable"
    "-Wno-unused-function"
    "-Wno-missing-field-initializers"
    "-Wno-sign-compare"
    "-I${LIBS_PATH}include"
    "-I${CODE_PATH}"
    "-g3"
    "-DENGINE_DEBUG=1"
)

# LINKER FLAGS FOR HOST
LDFLAGS=(
    "-Wl,--gc-sections"
    "-L${LIBS_PATH}lib"
    "-lpthread"
    "-ldl"
    "-lGL"
    "-lSDL2"
    "-lSDL2_ttf"
    "-lSDL2_image"
)

mkdir -p build/assets
cp -u "${LIBS_PATH}lib"/*.so build/ 2>/dev/null || true
cp -ru "${ASSETS_PATH}"* build/assets/ 2>/dev/null || true

# =============================================================================
# 1. BUILD GAME SHARED LIBRARY (Live-Reloadable)
# =============================================================================
# Set lock file during compilation
touch build/lock.tmp
echo "[BUILD] Compiling ${DLL_NAME}..."

g++ "${CXXFLAGS[@]}" -fPIC -shared "${CODE_PATH}game.cpp" -o "build/${DLL_NAME}"

rm -f build/lock.tmp

# =============================================================================
# 2. BUILD HOST RUNNER (Game)
# =============================================================================
if [ "$1" != "game" ]; then
    echo "[BUILD] Compiling ${EXE_NAME}..."
    g++ "${CXXFLAGS[@]}" "${CODE_PATH}sdl_main.cpp" "${LDFLAGS[@]}" -o "build/${EXE_NAME}"
fi

# =============================================================================
# 3. GENERATE QT CREATOR PROJECT FILES
# =============================================================================
touch "${PROJECT_NAME}.creator"
find "$CODE_PATH" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) > "${PROJECT_NAME}.files"
echo "${CODE_PATH}" > "${PROJECT_NAME}.includes"
echo "${LIBS_PATH}include" >> "${PROJECT_NAME}.includes"
printf "%s\n" "${CXXFLAGS[@]}" > "${PROJECT_NAME}.cxxflags"

mkdir -p .qtcreator
if [ ! -f ".qtcreator/${PROJECT_NAME}.creator.user" ] && [ -f "debug_template.user.backup" ]; then
    cp debug_template.user.backup ".qtcreator/${PROJECT_NAME}.creator.user"
fi

echo "[BUILD] Complete."