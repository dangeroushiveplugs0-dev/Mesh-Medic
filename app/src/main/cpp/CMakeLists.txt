cmake_minimum_required(VERSION 3.22.1)

project(meshmedic_native LANGUAGES CXX)

add_library(meshmedic_native SHARED
    native-lib.cpp
)

target_compile_features(meshmedic_native PRIVATE cxx_std_20)

target_compile_options(meshmedic_native PRIVATE
    -Wall
    -Wextra
    -Werror
)

find_library(log-lib log)

target_link_libraries(meshmedic_native
    ${log-lib}
)
