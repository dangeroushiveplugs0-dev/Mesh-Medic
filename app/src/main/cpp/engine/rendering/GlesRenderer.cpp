#include "GlesRenderer.h"

#include "CameraMath.h"

#include <GLES3/gl3.h>

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

constexpr float pi = 3.14159265358979323846f;

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

bool GlesRenderer::render(EglContext& context, int width, int height) {
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
    const Mat4 projection = Mat4::perspective(pi / 3.0f, aspect, 0.1f, 100.0f);
    const Mat4 view = Mat4::translation(0.0f, 0.0f, -5.0f);
    const Mat4 rotation = multiply(
        Mat4::rotation_y(0.65f),
        Mat4::rotation_x(-0.45f));
    const Mat4 model = rotation;
    const Mat4 model_view = multiply(view, model);
    const Mat4 mvp = multiply(projection, model_view);

    shader_.use();
    glUniformMatrix4fv(model_view_projection_, 1, GL_FALSE, mvp.value.data());
    cube_.draw();

    context.swapBuffers();
    return glGetError() == GL_NO_ERROR;
}

} // namespace meshmedic::rendering
