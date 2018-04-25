file(GLOB_RECURSE gb-emu-tests_SOURCES "*.cpp")

find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MAIN SDL2main)

add_executable(gb-emu-tests ${gb-emu-tests_SOURCES})

target_include_directories(gb-emu-tests PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(gb-emu-tests PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu-tests PRIVATE gb-emu-lib ${SDL2_LIBRARY} ${SDL2_MAIN} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
