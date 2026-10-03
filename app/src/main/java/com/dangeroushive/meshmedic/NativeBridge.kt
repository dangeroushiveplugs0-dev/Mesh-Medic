package com.dangeroushive.meshmedic

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("meshmedic_native")
    }

    external fun initializeCore(): Boolean
    external fun renderSurface(surface: Surface, width: Int, height: Int): Boolean
}
