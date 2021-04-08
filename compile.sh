#!/bin/bash
if [[ $1 == "" ]]; then
    echo "Please specify the full path to your VCPKG install."
else
    VCPKG_DIR=$1
    CMAKE_EXE=cmake

    rm -rf build
    mkdir build
    pushd build
    $VCPKG_DIR/vcpkg install sdl2
    $CMAKE_EXE .. "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" -G "Unix Makefiles"
    make
    popd
fi