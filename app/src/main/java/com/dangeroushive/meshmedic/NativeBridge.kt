package com.dangeroushive.meshmedic

import android.view.Surface

object NativeBridge {
    init {
        System.loadLibrary("meshmedic_native")
    }

    external fun initializeCore(): Boolean
    external fun startSurface(surface: Surface, width: Int, height: Int): Boolean
    external fun resizeSurface(width: Int, height: Int)
    external fun rotateCamera(
        startX: Float,
        startY: Float,
        endX: Float,
        endY: Float,
        width: Int,
        height: Int
    )
    external fun panCamera(
        deltaX: Float,
        deltaY: Float,
        width: Int,
        height: Int
    )
    external fun zoomCamera(scaleFactor: Float)
    external fun setCameraControlsEnabled(enabled: Boolean)
    external fun stopSurface()
}
