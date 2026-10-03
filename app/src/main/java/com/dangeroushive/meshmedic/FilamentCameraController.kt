package com.dangeroushive.meshmedic

import android.view.MotionEvent
import com.google.android.filament.Camera
import com.google.android.filament.utils.GestureDetector
import com.google.android.filament.utils.Manipulator

class FilamentCameraController(view: android.view.View) {

    private val manipulator = Manipulator.Builder()
        .targetPosition(0.0f, 0.0f, 0.0f)
        .orbitHomePosition(0.0f, 2.2f, 5.5f)
        .viewport(1, 1)
        .orbitSpeed(0.005f, 0.005f)
        .zoomSpeed(0.03f)
        .build(Manipulator.Mode.ORBIT)

    private val gestureDetector = GestureDetector(view, manipulator)

    private val eye = DoubleArray(3)
    private val target = DoubleArray(3)
    private val up = DoubleArray(3)

    fun setViewport(width: Int, height: Int) {
        if (width > 0 && height > 0) {
            manipulator.setViewport(width, height)
        }
    }

    fun onTouchEvent(event: MotionEvent): Boolean {
        return gestureDetector.onTouchEvent(event)
    }

    fun updateCamera(camera: Camera) {
        manipulator.getLookAt(eye, target, up)
        camera.lookAt(
            eye[0], eye[1], eye[2],
            target[0], target[1], target[2],
            up[0], up[1], up[2]
        )
    }
}
