#include <jni.h>

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_dangeroushive_meshmedic_NativeBridge_initializeCore(
        JNIEnv*, jobject) {
    return JNI_TRUE;
}
