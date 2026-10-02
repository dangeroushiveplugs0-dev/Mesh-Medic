#include "filament_renderer.h"

#include <filament/Camera.h>
#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/SwapChain.h>
#include <filament/View.h>

#include <utils/EntityManager.h>

#include <chrono>

namespace meshmedic {

FilamentRenderer::FilamentRenderer()
        : renderThread_(&FilamentRenderer::renderLoop, this) {}

FilamentRenderer::~FilamentRenderer() {
    running_.store(false);
    condition_.notify_all();

    if (renderThread_.joinable()) {
        renderThread_.join();
    }

    std::lock_guard<std::mutex> lock(mutex_);
    if (pendingWindow_) {
        ANativeWindow_release(pendingWindow_);
        pendingWindow_ = nullptr;
    }
}

void FilamentRenderer::setSurface(ANativeWindow* window) {
    if (!window) return;

    ANativeWindow_acquire(window);

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pendingWindow_) {
            ANativeWindow_release(pendingWindow_);
        }
        pendingWindow_ = window;
    }

    condition_.notify_all();
}

void FilamentRenderer::clearSurface() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pendingWindow_) {
            ANativeWindow_release(pendingWindow_);
            pendingWindow_ = nullptr;
        }
    }

    condition_.notify_all();
}

void FilamentRenderer::resize(int width, int height) {
    width_.store(width > 0 ? width : 1);
    height_.store(height > 0 ? height : 1);
    condition_.notify_all();
}

void FilamentRenderer::renderLoop() {
    using namespace filament;

    Engine* engine = Engine::create();
    Renderer* renderer = engine->createRenderer();
    Scene* scene = engine->createScene();
    View* view = engine->createView();

    auto& entityManager = utils::EntityManager::get();
    utils::Entity cameraEntity = entityManager.create();
    Camera* camera = engine->createCamera(cameraEntity);

    view->setScene(scene);
    view->setCamera(camera);

    renderer->setClearOptions({
        .clearColor = {0.055, 0.075, 0.105, 1.0},
        .clear = true,
        .discard = true
    });

    SwapChain* swapChain = nullptr;
    ANativeWindow* currentWindow = nullptr;

    while (running_.load()) {
        ANativeWindow* nextWindow = nullptr;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (pendingWindow_) {
                nextWindow = pendingWindow_;
                pendingWindow_ = nullptr;
            }
        }

        if (nextWindow) {
            if (swapChain) {
                engine->destroySwapChain(swapChain);
                engine->flushAndWait();
                swapChain = nullptr;
            }

            if (currentWindow) {
                ANativeWindow_release(currentWindow);
            }

            currentWindow = nextWindow;
            swapChain = engine->createSwapChain(currentWindow);
        }

        if (currentWindow && swapChain) {
            const int width = width_.load();
            const int height = height_.load();

            view->setViewport({
                0, 0,
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            });

            const double aspect = static_cast<double>(width) /
                                  static_cast<double>(height);

            camera->setProjection(
                Camera::Projection::PERSPECTIVE,
                45.0,
                aspect,
                0.1,
                100.0,
                Camera::Fov::VERTICAL);

            if (renderer->beginFrame(swapChain)) {
                renderer->render(view);
                renderer->endFrame();
            }
        }

        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait_for(lock, std::chrono::milliseconds(16));
    }

    if (swapChain) {
        engine->destroySwapChain(swapChain);
        engine->flushAndWait();
    }

    if (currentWindow) {
        ANativeWindow_release(currentWindow);
    }

    engine->destroy(view);
    engine->destroyCameraComponent(cameraEntity);
    entityManager.destroy(cameraEntity);
    engine->destroy(scene);
    engine->destroy(renderer);
    Engine::destroy(&engine);
}

} // namespace meshmedic
