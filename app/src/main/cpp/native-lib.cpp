#include <jni.h>
#include <android/native_window_jni.h>

#include <memory>
#include <mutex>

#include "engine/rendering/RenderThread.h"

namespace meshmedic {

std::mutex render_mutex;
std::unique_ptr<rendering::RenderThread> render_thread;
ANativeWindow* render_window = nullptr;

bool initialize_core() noexcept {
    return true;
}

bool start_surface(JNIEnv* env, jobject surface, jint width, jint height) {
    if (surface == nullptr || width <= 0 || height <= 0) {
        return false;
    }

    std::lock_guard lock(render_mutex);

    if (render_thread) {
        render_thread->stop();
        render_thread.reset();
    }

    if (render_window != nullptr) {
        ANativeWindow_release(render_window);
        render_window = nullptr;
    }

    render_window = ANativeWindow_fromSurface(env, surface);
    if (render_window == nullptr) {
        return false;
    }

    render_thread = std::make_unique<rendering::RenderThread>();
    if (!render_thread->start(render_window, width, height)) {
        render_thread.reset();
        ANativeWindow_release(render_window);
        render_window = nullptr;
        return false;
    }

    return true;
}

void resize_surface(jint width, jint height) {
    std::lock_guard lock(render_mutex);
    if (render_thread) {
        render_thread->resize(width, height);
    }
}

void rotate_camera(
    jfloat start_x,
    jfloat start_y,
    jfloat end_x,
    jfloat end_y,
    jint width,
    jint height) {
    std::lock_guard lock(render_mutex);
    if (render_thread) {
        render_thread->rotateCamera(
            start_x, start_y, end_x, end_y, width, height);
    }
}

void pan_camera(
    jfloat delta_x,
    jfloat delta_y,
    jint width,
    jint height) {
    std::lock_guard lock(render_mutex);
    if (render_thread) {
        render_thread->panCamera(delta_x, delta_y, width, height);
    }
}

void zoom_camera(jfloat scale_factor) {
    std::lock_guard lock(render_mutex);
    if (render_thread) {
        render_thread->zoomCamera(scale_factor);
    }
}

void set_camera_controls_enabled(bool enabled) {
    std::lock_guard lock(render_mutex);
    if (render_thread) {
        render_thread->setCameraControlsEnabled(enabled);
    }
}

void stop_surface() {
    std::lock_guard lock(render_mutex);

    if (render_thread) {
        render_thread->stop();
        render_thread.reset();
    }

    if (render_window != nullptr) {
        ANativeWindow_release(render_window);
        render_window = nullptr;
    }
}

} // namespace meshmedic

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_initializeCore(
    JNIEnv*,
    jobject) {
    return meshmedic::initialize_core() ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_startSurface(
    JNIEnv* env,
    jobject,
    jobject surface,
    jint width,
    jint height) {
    return meshmedic::start_surface(env, surface, width, height)
        ? JNI_TRUE
        : JNI_FALSE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_resizeSurface(
    JNIEnv*,
    jobject,
    jint width,
    jint height) {
    meshmedic::resize_surface(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_rotateCamera(
    JNIEnv*,
    jobject,
    jfloat start_x,
    jfloat start_y,
    jfloat end_x,
    jfloat end_y,
    jint width,
    jint height) {
    meshmedic::rotate_camera(
        start_x, start_y, end_x, end_y, width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_panCamera(
    JNIEnv*,
    jobject,
    jfloat delta_x,
    jfloat delta_y,
    jint width,
    jint height) {
    meshmedic::pan_camera(delta_x, delta_y, width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_zoomCamera(
    JNIEnv*,
    jobject,
    jfloat scale_factor) {
    meshmedic::zoom_camera(scale_factor);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_setCameraControlsEnabled(
    JNIEnv*,
    jobject,
    jboolean enabled) {
    meshmedic::set_camera_controls_enabled(enabled == JNI_TRUE);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_stopSurface(
    JNIEnv*,
    jobject) {
    meshmedic::stop_surface();
}
