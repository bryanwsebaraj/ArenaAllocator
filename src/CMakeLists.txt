cmake_minimum_required(VERSION 3.27)

add_library(ArenaAllocator SHARED memOS.cpp)

target_compile_options(ArenaAllocator PRIVATE -fPIC)

set_target_properties(ArenaAllocator PROPERTIES OUTPUT_NAME "arenaallocator")