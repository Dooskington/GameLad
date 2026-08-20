file(GLOB_RECURSE gb-emu_SOURCES "*.cpp")

find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MAIN SDL2main)

if (NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY)
    message(FATAL_ERROR "SDL2 headers and library are required for the desktop frontend")
endif()

add_executable(gb-emu ${gb-emu_SOURCES})

target_include_directories(gb-emu PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(gb-emu PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu PRIVATE gb-emu-lib ${SDL2_LIBRARY} ${CMAKE_DL_LIBS})

if (SDL2_MAIN)
    target_link_libraries(gb-emu PRIVATE ${SDL2_MAIN})
endif()
