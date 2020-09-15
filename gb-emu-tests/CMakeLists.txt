file(GLOB_RECURSE gb-emu-tests_SOURCES "*.cpp")

find_package(SDL2 REQUIRED)

add_executable(gb-emu-tests ${gb-emu-tests_SOURCES})

target_include_directories(gb-emu-tests PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(gb-emu-tests PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu-tests PRIVATE gb-emu-lib SDL2::SDL2 SDL2::SDL2main ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
