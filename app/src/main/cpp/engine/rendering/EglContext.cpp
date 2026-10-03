#include "EglContext.h"

namespace meshmedic::rendering {

EglContext::~EglContext() {
    destroy();
}

bool EglContext::initialize(EGLNativeWindowType window) {
    destroy();

    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        return false;
    }

    if (eglInitialize(display_, nullptr, nullptr) == EGL_FALSE) {
        destroy();
        return false;
    }

    constexpr EGLint config_attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };

    EGLConfig config = nullptr;
    EGLint config_count = 0;
    if (eglChooseConfig(display_, config_attributes, &config, 1, &config_count) == EGL_FALSE ||
        config_count == 0) {
        destroy();
        return false;
    }

    constexpr EGLint context_attributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, context_attributes);
    if (context_ == EGL_NO_CONTEXT) {
        destroy();
        return false;
    }

    surface_ = eglCreateWindowSurface(display_, config, window, nullptr);
    if (surface_ == EGL_NO_SURFACE || !makeCurrent()) {
        destroy();
        return false;
    }

    return true;
}

void EglContext::destroy() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
        }

        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
        }

        eglTerminate(display_);
    }

    display_ = EGL_NO_DISPLAY;
    surface_ = EGL_NO_SURFACE;
    context_ = EGL_NO_CONTEXT;
}

bool EglContext::makeCurrent() {
    return display_ != EGL_NO_DISPLAY &&
           surface_ != EGL_NO_SURFACE &&
           context_ != EGL_NO_CONTEXT &&
           eglMakeCurrent(display_, surface_, surface_, context_) == EGL_TRUE;
}

void EglContext::swapBuffers() {
    if (display_ != EGL_NO_DISPLAY && surface_ != EGL_NO_SURFACE) {
        eglSwapBuffers(display_, surface_);
    }
}

} // namespace meshmedic::rendering
