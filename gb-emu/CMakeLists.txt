file(GLOB_RECURSE gb-emu_SOURCES "*.cpp")

find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MAIN SDL2main)

add_executable(gb-emu ${gb-emu_SOURCES})

target_include_directories(gb-emu PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(gb-emu PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu PRIVATE gb-emu-lib ${SDL2_LIBRARY} ${SDL2_MAIN} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
