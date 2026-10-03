#pragma once

#include "CubeMesh.h"
#include "EglContext.h"
#include "ShaderProgram.h"

#include <glm/mat4x4.hpp>

namespace meshmedic::rendering {

class GlesRenderer {
public:
    GlesRenderer() = default;
    ~GlesRenderer();

    GlesRenderer(const GlesRenderer&) = delete;
    GlesRenderer& operator=(const GlesRenderer&) = delete;

    bool initialize(EglContext& context);
    void destroy();
    bool render(EglContext& context, int width, int height, const glm::mat4& view);

private:
    ShaderProgram shader_;
    CubeMesh cube_;
    GLint model_view_projection_ = -1;
};

} // namespace meshmedic::rendering
