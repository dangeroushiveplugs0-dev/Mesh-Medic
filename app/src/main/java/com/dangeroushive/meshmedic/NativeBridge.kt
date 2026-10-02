package com.dangeroushive.meshmedic

object NativeBridge {
    init {
        System.loadLibrary("meshmedic_native")
    }

    external fun initializeCore(): Boolean
}
