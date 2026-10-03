cmake_minimum_required(VERSION 3.22.1)

project(meshmedic_native LANGUAGES CXX)

include(FetchContent)

set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)

add_library(meshmedic_native SHARED
    native-lib.cpp
    engine/rendering/ArcballCamera.cpp
    engine/rendering/EglContext.cpp
    engine/rendering/ShaderProgram.cpp
    engine/rendering/CubeMesh.cpp
    engine/rendering/GlesRenderer.cpp
    engine/rendering/RenderThread.cpp
)

target_compile_features(meshmedic_native PRIVATE cxx_std_20)

target_compile_options(meshmedic_native PRIVATE
    -Wall
    -Wextra
    -Werror
)

target_link_libraries(meshmedic_native PRIVATE
    glm::glm
)

find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(glesv3-lib GLESv3)

target_link_libraries(meshmedic_native PRIVATE
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${glesv3-lib}
)
