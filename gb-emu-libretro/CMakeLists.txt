add_library(gamelad_libretro SHARED LibretroCore.cpp)

target_compile_features(gamelad_libretro PRIVATE cxx_std_14)
target_include_directories(
    gamelad_libretro
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/gb-emu-lib)
target_link_libraries(gamelad_libretro PRIVATE gb-emu-lib)

set_target_properties(
    gamelad_libretro
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        PREFIX ""
        OUTPUT_NAME "gamelad_libretro"
        VISIBILITY_INLINES_HIDDEN YES)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gamelad_libretro.info
    ${CMAKE_BINARY_DIR}/bin/gamelad_libretro.info
    COPYONLY)
