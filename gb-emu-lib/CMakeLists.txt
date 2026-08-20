file(GLOB_RECURSE gb-emu-lib_SOURCES "*.cpp")

add_library(gb-emu-lib ${gb-emu-lib_SOURCES})
target_compile_features(gb-emu-lib PUBLIC cxx_std_14)
set_target_properties(gb-emu-lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
