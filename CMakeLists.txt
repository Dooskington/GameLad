# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(GameLad)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (NOT WIN32)
    find_package (Threads)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    add_definitions(-lstdc++ -ldl -lfbxsdk -lpng -lz -lfreetype)
endif(NOT WIN32)

add_subdirectory(gb-emu-lib)
add_subdirectory(gb-emu-tests)
add_subdirectory(gb-emu)

if (WIN32)
    file(COPY ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/sdl2.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif(WIN32)