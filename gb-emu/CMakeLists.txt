file(GLOB_RECURSE gb-emu_SOURCES "*.cpp")

find_package(SDL2 REQUIRED)

add_executable(gb-emu ${gb-emu_SOURCES})

target_include_directories(gb-emu PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(gb-emu PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu PRIVATE gb-emu-lib SDL2::SDL2 SDL2::SDL2main ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})
