add_executable(gb-emu-libretro-tests Main.cpp)

target_compile_features(gb-emu-libretro-tests PRIVATE cxx_std_14)
target_compile_definitions(gb-emu-libretro-tests PRIVATE RETRO_IMPORT_SYMBOLS)
target_include_directories(
    gb-emu-libretro-tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/gb-emu-libretro)
target_link_libraries(gb-emu-libretro-tests PRIVATE gamelad_libretro)
