#include "GlesRenderer.h"

#include <GLES3/gl3.h>

namespace meshmedic::rendering {

bool GlesRenderer::render(EglContext& context, int width, int height) {
    if (!context.makeCurrent() || width <= 0 || height <= 0) {
        return false;
    }

    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.055f, 0.055f, 0.065f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    context.swapBuffers();
    return glGetError() == GL_NO_ERROR;
}

} // namespace meshmedic::rendering
