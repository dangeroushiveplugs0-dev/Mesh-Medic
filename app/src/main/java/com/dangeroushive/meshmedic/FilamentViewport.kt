package com.dangeroushive.meshmedic

import android.content.Context
import android.view.SurfaceView

class FilamentViewport(context: Context) : SurfaceView(context) {
    init {
        isFocusable = true
        isFocusableInTouchMode = true
    }
}
