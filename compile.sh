#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Please specify the full path to your VCPKG install."
else
    VCPKG_DIR=$1
    VCPKG_TOOLS_DIR=$VCPKG_DIR/downloads/tools
    CMAKE_EXE=$VCPKG_TOOLS_DIR/cmake-3.17.2-osx/cmake-3.17.2-Darwin-x86_64/CMake.app/Contents/bin/cmake

    rm -rf build
    mkdir build
    pushd build
    $VCPKG_DIR/vcpkg install sdl2
    $CMAKE_EXE .. "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" -G "Unix Makefiles"
    make
    popd
fi