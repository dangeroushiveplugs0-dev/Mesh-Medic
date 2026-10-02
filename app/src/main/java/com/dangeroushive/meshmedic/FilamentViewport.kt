package com.dangeroushive.meshmedic

import android.content.Context
import android.view.Choreographer
import android.view.Surface
import android.view.SurfaceView
import com.google.android.filament.Camera
import com.google.android.filament.Engine
import com.google.android.filament.EntityManager
import com.google.android.filament.Filament
import com.google.android.filament.Renderer
import com.google.android.filament.Scene
import com.google.android.filament.SwapChain
import com.google.android.filament.View
import com.google.android.filament.Viewport
import com.google.android.filament.android.UiHelper

class FilamentViewport(context: Context) : SurfaceView(context) {

    private val choreographer = Choreographer.getInstance()
    private val uiHelper = UiHelper(UiHelper.ContextErrorPolicy.DONT_CHECK)

    private val engine: Engine
    private val renderer: Renderer
    private val scene: Scene
    private val filamentView: View
    private val camera: Camera

    private var swapChain: SwapChain? = null
    private var running = false

    private val frameCallback = object : Choreographer.FrameCallback {
        override fun doFrame(frameTimeNanos: Long) {
            if (!running) return

            choreographer.postFrameCallback(this)

            val chain = swapChain ?: return
            if (renderer.beginFrame(chain, frameTimeNanos)) {
                renderer.render(filamentView)
                renderer.endFrame()
            }
        }
    }

    init {
        Filament.init()

        engine = Engine.create()
        renderer = engine.createRenderer()
        scene = engine.createScene()
        filamentView = engine.createView()
        camera = engine.createCamera(EntityManager.get().create())

        filamentView.camera = camera
        filamentView.scene = scene

        renderer.clearOptions = Renderer.ClearOptions().apply {
            clearColor = floatArrayOf(0.055f, 0.075f, 0.105f, 1.0f)
            clear = true
            discard = true
        }

        uiHelper.renderCallback = object : UiHelper.RendererCallback {
            override fun onNativeWindowChanged(surface: Surface) {
                swapChain?.let { engine.destroySwapChain(it) }
                swapChain = engine.createSwapChain(surface, uiHelper.swapChainFlags)
            }

            override fun onDetachedFromSurface() {
                swapChain?.let {
                    engine.destroySwapChain(it)
                    engine.flushAndWait()
                    swapChain = null
                }
            }

            override fun onResized(width: Int, height: Int) {
                if (height <= 0) return

                filamentView.viewport = Viewport(0, 0, width, height)

                camera.setProjection(
                    Camera.Projection.PERSPECTIVE,
                    45.0,
                    width.toDouble() / height.toDouble(),
                    0.1,
                    100.0,
                    Camera.Fov.VERTICAL
                )
            }
        }

        uiHelper.attachTo(this)
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        running = true
        choreographer.postFrameCallback(frameCallback)
    }

    override fun onDetachedFromWindow() {
        running = false
        choreographer.removeFrameCallback(frameCallback)
        uiHelper.detach()

        engine.destroyView(filamentView)
        engine.destroyScene(scene)
        engine.destroyCameraComponent(camera.entity)
        engine.destroyEntity(camera.entity)
        engine.destroyRenderer(renderer)
        engine.destroy()

        super.onDetachedFromWindow()
    }
}
