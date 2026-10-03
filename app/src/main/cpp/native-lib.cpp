#include <jni.h>
#include <android/native_window_jni.h>

#include "engine/rendering/EglContext.h"
#include "engine/rendering/GlesRenderer.h"

namespace meshmedic {

bool initialize_core() noexcept {
    return true;
}

bool render_surface(JNIEnv* env, jobject surface, jint width, jint height) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        return false;
    }

    rendering::EglContext context;
    const bool initialized = context.initialize(window);
    const bool rendered = initialized &&
        rendering::GlesRenderer{}.render(context, width, height);

    ANativeWindow_release(window);
    return rendered;
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
Java_com_dangeroushive_meshmedic_NativeBridge_renderSurface(
    JNIEnv* env,
    jobject,
    jobject surface,
    jint width,
    jint height) {
    return meshmedic::render_surface(env, surface, width, height)
        ? JNI_TRUE
        : JNI_FALSE;
}
