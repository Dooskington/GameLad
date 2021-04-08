file(GLOB_RECURSE gb-emu-lib_SOURCES "*.cpp")

find_package(SDL2 REQUIRED)

add_library(gb-emu-lib ${gb-emu-lib_SOURCES})
target_include_directories(gb-emu-lib PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(gb-emu-lib PRIVATE SDL2::SDL2 SDL2::SDL2main ${CMAKE_DL_LIBS})
