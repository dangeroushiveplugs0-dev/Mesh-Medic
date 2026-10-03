#include <jni.h>

namespace meshmedic {

bool initialize_core() noexcept {
    return true;
}

} // namespace meshmedic

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_initializeCore(
    JNIEnv*,
    jobject) {
    return meshmedic::initialize_core() ? JNI_TRUE : JNI_FALSE;
}
