file(GLOB_RECURSE gb-emu-tests_SOURCES "*.cpp")

add_executable(gb-emu-tests ${gb-emu-tests_SOURCES})

target_include_directories(gb-emu-tests PUBLIC ../gb-emu-lib)

target_link_libraries(gb-emu-tests PRIVATE gb-emu-lib)

if (MSVC)
    # TestMain.cpp includes every test implementation; optimizing that harness
    # adds substantial compile time without improving the shipped binaries.
    target_compile_options(gb-emu-tests PRIVATE /Od)
endif()
