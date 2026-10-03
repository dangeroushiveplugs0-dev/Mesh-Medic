#pragma once

#include <EGL/egl.h>

namespace meshmedic::rendering {

class EglContext {
public:
    EglContext() = default;
    ~EglContext();

    EglContext(const EglContext&) = delete;
    EglContext& operator=(const EglContext&) = delete;

    bool initialize(EGLNativeWindowType window);
    void destroy();
    bool makeCurrent();
    void swapBuffers();

private:
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
};

} // namespace meshmedic::rendering
