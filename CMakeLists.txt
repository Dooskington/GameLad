# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(GameLad)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(GAMELAD_BUILD_SDL_FRONTEND "Build the SDL desktop frontend" ${WIN32})

add_subdirectory(gb-emu-lib)
add_subdirectory(gb-emu-libretro)
add_subdirectory(gb-emu-libretro-tests)
add_subdirectory(gb-emu-tests)

if (GAMELAD_BUILD_SDL_FRONTEND)
    add_subdirectory(gb-emu)
endif()

if (WIN32 AND GAMELAD_BUILD_SDL_FRONTEND)
    file(COPY ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/sdl2.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()