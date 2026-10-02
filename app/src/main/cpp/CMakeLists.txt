cmake_minimum_required(VERSION 3.22.1)

project("meshmedic_native")

add_library(
    meshmedic_native
    SHARED
    native-lib.cpp
)

target_compile_features(meshmedic_native PRIVATE cxx_std_20)

target_link_libraries(
    meshmedic_native
    PRIVATE
    android
    log
)
