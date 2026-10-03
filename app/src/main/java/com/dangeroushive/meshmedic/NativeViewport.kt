package com.dangeroushive.meshmedic

import android.content.Context
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import kotlin.math.hypot

class NativeViewport(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    private var nativeSurfaceStarted = false
    private var cameraControlsEnabled = true

    private var activePointerId = MotionEvent.INVALID_POINTER_ID
    private var lastX = 0f
    private var lastY = 0f

    private var pinchPointerIdA = MotionEvent.INVALID_POINTER_ID
    private var pinchPointerIdB = MotionEvent.INVALID_POINTER_ID
    private var lastPinchDistance = 0f
    private var lastPinchCenterX = 0f
    private var lastPinchCenterY = 0f

    init {
        holder.addCallback(this)
        isFocusable = true
    }

    override fun surfaceCreated(holder: SurfaceHolder) = Unit

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        if (nativeSurfaceStarted) {
            NativeBridge.resizeSurface(width, height)
        } else {
            nativeSurfaceStarted = NativeBridge.startSurface(holder.surface, width, height)
        }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (!cameraControlsEnabled) {
            resetGestureState()
            return false
        }

        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                activePointerId = event.getPointerId(0)
                lastX = event.getX(0)
                lastY = event.getY(0)
                return true
            }

            MotionEvent.ACTION_POINTER_DOWN -> {
                if (event.pointerCount >= 2) {
                    val indexA = 0
                    val indexB = 1

                    pinchPointerIdA = event.getPointerId(indexA)
                    pinchPointerIdB = event.getPointerId(indexB)
                    lastPinchDistance = distance(event, indexA, indexB)

                    val center = center(event, indexA, indexB)
                    lastPinchCenterX = center.first
                    lastPinchCenterY = center.second
                }
                return true
            }

            MotionEvent.ACTION_MOVE -> {
                if (event.pointerCount == 1) {
                    val pointerIndex = event.findPointerIndex(activePointerId)
                    if (pointerIndex < 0) {
                        return true
                    }

                    val x = event.getX(pointerIndex)
                    val y = event.getY(pointerIndex)

                    NativeBridge.rotateCamera(
                        lastX,
                        lastY,
                        x,
                        y,
                        width,
                        height
                    )

                    lastX = x
                    lastY = y
                    return true
                }

                if (event.pointerCount >= 2) {
                    val indexA = event.findPointerIndex(pinchPointerIdA)
                    val indexB = event.findPointerIndex(pinchPointerIdB)
                    if (indexA < 0 || indexB < 0) {
                        return true
                    }

                    val currentDistance = distance(event, indexA, indexB)
                    if (lastPinchDistance > 0f && currentDistance > 0f) {
                        NativeBridge.zoomCamera(
                            currentDistance / lastPinchDistance
                        )
                    }

                    val center = center(event, indexA, indexB)
                    NativeBridge.panCamera(
                        center.first - lastPinchCenterX,
                        center.second - lastPinchCenterY,
                        width,
                        height
                    )

                    lastPinchDistance = currentDistance
                    lastPinchCenterX = center.first
                    lastPinchCenterY = center.second
                    return true
                }
            }

            MotionEvent.ACTION_POINTER_UP -> {
                val liftedId = event.getPointerId(event.actionIndex)

                if (liftedId == pinchPointerIdA || liftedId == pinchPointerIdB) {
                    val remainingIndex = if (event.actionIndex == 0) 1 else 0
                    if (remainingIndex < event.pointerCount) {
                        activePointerId = event.getPointerId(remainingIndex)
                        lastX = event.getX(remainingIndex)
                        lastY = event.getY(remainingIndex)
                    }
                    pinchPointerIdA = MotionEvent.INVALID_POINTER_ID
                    pinchPointerIdB = MotionEvent.INVALID_POINTER_ID
                    lastPinchDistance = 0f
                }
                return true
            }

            MotionEvent.ACTION_UP,
            MotionEvent.ACTION_CANCEL -> {
                resetGestureState()
                return true
            }
        }

        return true
    }

    fun setCameraControlsEnabled(enabled: Boolean) {
        cameraControlsEnabled = enabled
        NativeBridge.setCameraControlsEnabled(enabled)
        if (!enabled) {
            resetGestureState()
        }
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        NativeBridge.stopSurface()
        nativeSurfaceStarted = false
        resetGestureState()
    }

    private fun resetGestureState() {
        activePointerId = MotionEvent.INVALID_POINTER_ID
        pinchPointerIdA = MotionEvent.INVALID_POINTER_ID
        pinchPointerIdB = MotionEvent.INVALID_POINTER_ID
        lastPinchDistance = 0f
    }

    private fun distance(event: MotionEvent, indexA: Int, indexB: Int): Float {
        return hypot(
            event.getX(indexA) - event.getX(indexB),
            event.getY(indexA) - event.getY(indexB)
        )
    }

    private fun center(event: MotionEvent, indexA: Int, indexB: Int): Pair<Float, Float> {
        return Pair(
            (event.getX(indexA) + event.getX(indexB)) * 0.5f,
            (event.getY(indexA) + event.getY(indexB)) * 0.5f
        )
    }
}
