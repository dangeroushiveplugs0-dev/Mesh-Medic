package com.dangeroushive.meshmedic

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView

class NativeViewport(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    private var nativeSurfaceStarted = false

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) = Unit

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        if (nativeSurfaceStarted) {
            NativeBridge.resizeSurface(width, height)
        } else {
            nativeSurfaceStarted = NativeBridge.startSurface(holder.surface, width, height)
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        NativeBridge.stopSurface()
        nativeSurfaceStarted = false
    }
}
