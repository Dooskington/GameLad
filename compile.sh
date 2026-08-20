#!/usr/bin/env bash
set -euo pipefail

build_type=Debug
if [[ $# -gt 1 ]]; then
    echo "Usage: ./compile.sh [--release]"
    exit 1
fi

if [[ $# -eq 1 ]]; then
    if [[ $1 != "--release" ]]; then
        echo "Unknown option: $1"
        echo "Usage: ./compile.sh [--release]"
        exit 1
    fi
    build_type=Release
fi

cmake -S . -B build-linux \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DGAMELAD_BUILD_SDL_FRONTEND=OFF
cmake --build build-linux --parallel

if [[ ! -f build-linux/bin/gamelad_libretro.so ]]; then
    echo "Expected libretro core was not created: build-linux/bin/gamelad_libretro.so"
    exit 1
fi

if [[ ! -f build-linux/bin/gamelad_libretro.info ]]; then
    echo "Expected core metadata was not created: build-linux/bin/gamelad_libretro.info"
    exit 1
fi

echo "Built $build_type libretro core and metadata in build-linux/bin"