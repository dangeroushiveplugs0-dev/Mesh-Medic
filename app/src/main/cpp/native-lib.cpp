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
Java_com_dangeroushive_meshmedic_NativeBridge_stopSurface(
    JNIEnv*,
    jobject) {
    meshmedic::stop_surface();
}
