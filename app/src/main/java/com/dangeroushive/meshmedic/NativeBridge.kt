package com.dangeroushive.meshmedic

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("meshmedic_native")
    }

    external fun initializeCore(): Boolean
    external fun startSurface(surface: Surface, width: Int, height: Int): Boolean
    external fun resizeSurface(width: Int, height: Int)
    external fun stopSurface()
}
