package com.dangeroushive.meshmedic

import android.view.MotionEvent
import com.google.android.filament.Camera
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.max
import kotlin.math.min
import kotlin.math.sin
import kotlin.math.sqrt

class OrbitCameraController(
    private val camera: Camera
) {
    private var targetX = 0.0
    private var targetY = 0.0
    private var targetZ = 0.0

    private var yaw = Math.toRadians(0.0)
    private var pitch = Math.toRadians(-8.0)
    private var distance = 5.5

    private var lastX = 0f
    private var lastY = 0f
    private var lastSpan = 0f
    private var lastCentroidX = 0f
    private var lastCentroidY = 0f

    private var activePointers = 0

    init {
        apply()
    }

    fun reset() {
        targetX = 0.0
        targetY = 0.0
        targetZ = 0.0
        yaw = 0.0
        pitch = Math.toRadians(-8.0)
        distance = 5.5
        apply()
    }

    fun onTouchEvent(event: MotionEvent): Boolean {
        when (event.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                activePointers = 1
                lastX = event.x
                lastY = event.y
                return true
            }

            MotionEvent.ACTION_POINTER_DOWN -> {
                activePointers = event.pointerCount
                if (event.pointerCount >= 2) {
                    lastSpan = span(event)
                    val centroid = centroid(event)
                    lastCentroidX = centroid.first
                    lastCentroidY = centroid.second
                }
                return true
            }

            MotionEvent.ACTION_MOVE -> {
                if (event.pointerCount >= 2) {
                    activePointers = event.pointerCount

                    val centroid = centroid(event)
                    val dx = centroid.first - lastCentroidX
                    val dy = centroid.second - lastCentroidY

                    pan(dx, dy)

                    val currentSpan = span(event)
                    if (lastSpan > 0f && currentSpan > 0f) {
                        val scale = currentSpan / lastSpan
                        distance = (distance / scale.toDouble()).coerceIn(2.0, 25.0)
                    }

                    lastCentroidX = centroid.first
                    lastCentroidY = centroid.second
                    lastSpan = currentSpan
                    apply()
                } else {
                    val dx = event.x - lastX
                    val dy = event.y - lastY

                    yaw -= dx * 0.008
                    pitch = (pitch - dy * 0.008).coerceIn(
                        Math.toRadians(-85.0),
                        Math.toRadians(85.0)
                    )

                    lastX = event.x
                    lastY = event.y
                    apply()
                }
                return true
            }

            MotionEvent.ACTION_POINTER_UP -> {
                activePointers = max(1, event.pointerCount - 1)
                val remainingIndex = if (event.actionIndex == 0) 1 else 0
                if (remainingIndex < event.pointerCount) {
                    lastX = event.getX(remainingIndex)
                    lastY = event.getY(remainingIndex)
                }
                return true
            }

            MotionEvent.ACTION_UP,
            MotionEvent.ACTION_CANCEL -> {
                activePointers = 0
                return true
            }
        }

        return true
    }

    private fun pan(dx: Float, dy: Float) {
        val panScale = distance * 0.0018

        val forwardX = cos(pitch) * sin(yaw)
        val forwardY = sin(pitch)
        val forwardZ = cos(pitch) * cos(yaw)

        var rightX = forwardZ
        var rightZ = -forwardX
        val rightLength = sqrt(rightX * rightX + rightZ * rightZ)

        if (rightLength > 0.0001) {
            rightX /= rightLength
            rightZ /= rightLength
        }

        val upX = -sin(pitch) * sin(yaw)
        val upY = cos(pitch)
        val upZ = -sin(pitch) * cos(yaw)

        targetX -= (rightX * dx - upX * dy) * panScale
        targetY -= (0.0 * dx - upY * dy) * panScale
        targetZ -= (rightZ * dx - upZ * dy) * panScale
    }

    private fun apply() {
        val cosPitch = cos(pitch)

        val cameraX = targetX + distance * cosPitch * sin(yaw)
        val cameraY = targetY + distance * sin(pitch)
        val cameraZ = targetZ + distance * cosPitch * cos(yaw)

        camera.lookAt(
            cameraX, cameraY, cameraZ,
            targetX, targetY, targetZ,
            0.0, 1.0, 0.0
        )
    }

    private fun span(event: MotionEvent): Float {
        if (event.pointerCount < 2) return 0f
        val dx = event.getX(0) - event.getX(1)
        val dy = event.getY(0) - event.getY(1)
        return sqrt(dx * dx + dy * dy)
    }

    private fun centroid(event: MotionEvent): Pair<Float, Float> {
        if (event.pointerCount < 2) {
            return event.x to event.y
        }

        return (
            (event.getX(0) + event.getX(1)) * 0.5f
        ) to (
            (event.getY(0) + event.getY(1)) * 0.5f
        )
    }
}
