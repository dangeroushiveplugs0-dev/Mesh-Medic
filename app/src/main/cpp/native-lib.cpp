#include <jni.h>
#include <android/native_window_jni.h>

#include "filament_renderer.h"

namespace {
meshmedic::FilamentRenderer* gRenderer = nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_initializeCore(
        JNIEnv*, jobject) {
    if (!gRenderer) {
        gRenderer = new meshmedic::FilamentRenderer();
    }
    return JNI_TRUE;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_setSurface(
        JNIEnv* env, jobject, jobject surface) {
    if (!gRenderer || !surface) return;

    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (!window) return;

    gRenderer->setSurface(window);
    ANativeWindow_release(window);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_clearSurface(
        JNIEnv*, jobject) {
    if (gRenderer) {
        gRenderer->clearSurface();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_resizeSurface(
        JNIEnv*, jobject, jint width, jint height) {
    if (gRenderer) {
        gRenderer->resize(width, height);
    }
}
