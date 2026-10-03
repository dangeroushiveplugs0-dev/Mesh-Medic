cmake_minimum_required(VERSION 3.22.1)

project(meshmedic_native LANGUAGES CXX)

add_library(meshmedic_native SHARED
    native-lib.cpp
    engine/rendering/EglContext.cpp
    engine/rendering/GlesRenderer.cpp
    engine/rendering/RenderThread.cpp
)

target_compile_features(meshmedic_native PRIVATE cxx_std_20)

target_compile_options(meshmedic_native PRIVATE
    -Wall
    -Wextra
    -Werror
)

find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(glesv3-lib GLESv3)

target_link_libraries(meshmedic_native
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${glesv3-lib}
)
