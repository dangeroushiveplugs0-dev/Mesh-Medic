package com.dangeroushive.meshmedic

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView

class FilamentViewport(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    init {
        isFocusable = true
        isFocusableInTouchMode = true
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        NativeBridge.setSurface(holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        NativeBridge.resizeSurface(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        NativeBridge.clearSurface()
    }
}
