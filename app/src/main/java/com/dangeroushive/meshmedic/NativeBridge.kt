package com.dangeroushive.meshmedic

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("meshmedic_native")
    }

    external fun initializeCore(): Boolean
    external fun setSurface(surface: Surface)
    external fun clearSurface()
    external fun resizeSurface(width: Int, height: Int)
}
