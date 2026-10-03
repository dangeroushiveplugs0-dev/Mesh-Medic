#include "GlesRenderer.h"

#include <GLES3/gl3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>

namespace meshmedic::rendering {

namespace {

constexpr char vertex_shader[] = R"(
#version 300 es
precision highp float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

uniform mat4 u_model_view_projection;

out vec3 v_color;

void main() {
    gl_Position = u_model_view_projection * vec4(a_position, 1.0);
    v_color = a_color;
}
)";

constexpr char fragment_shader[] = R"(
#version 300 es
precision mediump float;

in vec3 v_color;
out vec4 out_color;

void main() {
    out_color = vec4(v_color, 1.0);
}
)";

} // namespace

GlesRenderer::~GlesRenderer() {
    destroy();
}

bool GlesRenderer::initialize(EglContext& context) {
    destroy();

    if (!context.makeCurrent()) {
        return false;
    }

    if (!shader_.initialize(vertex_shader, fragment_shader)) {
        destroy();
        return false;
    }

    if (!cube_.initialize()) {
        destroy();
        return false;
    }

    model_view_projection_ = shader_.uniform_location("u_model_view_projection");
    if (model_view_projection_ < 0) {
        destroy();
        return false;
    }

    return glGetError() == GL_NO_ERROR;
}

void GlesRenderer::destroy() {
    cube_.destroy();
    shader_.destroy();
    model_view_projection_ = -1;
}

bool GlesRenderer::render(
    EglContext& context,
    int width,
    int height,
    const glm::mat4& view) {
    if (!context.makeCurrent() || width <= 0 || height <= 0 ||
        shader_.id() == 0 || model_view_projection_ < 0) {
        return false;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.055f, 0.055f, 0.065f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const glm::mat4 projection = glm::perspectiveRH_NO(
        glm::radians(60.0f),
        aspect,
        0.1f,
        100.0f);

    const glm::mat4 model = glm::mat4(1.0f);
    const glm::mat4 mvp = projection * view * model;

    shader_.use();
    glUniformMatrix4fv(model_view_projection_, 1, GL_FALSE, &mvp[0][0]);
    cube_.draw();

    context.swapBuffers();
    return glGetError() == GL_NO_ERROR;
}

} // namespace meshmedic::rendering
