file(GLOB_RECURSE gb-emu-lib_SOURCES "*.cpp")

find_path(SDL2_INCLUDE_DIR SDL2/SDL.h)
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_MAIN SDL2main)

add_library(gb-emu-lib ${gb-emu-lib_SOURCES})
target_include_directories(gb-emu-lib PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(gb-emu-lib PRIVATE ${SDL2_LIBRARY} ${SDL2_MAIN} ${CMAKE_DL_LIBS})
