#include "RenderThread.h"

#include <chrono>
#include <glm/mat4x4.hpp>

namespace meshmedic::rendering {

RenderThread::~RenderThread() {
    stop();
}

bool RenderThread::start(ANativeWindow* window, int width, int height) {
    if (window == nullptr || width <= 0 || height <= 0 || running_.load()) {
        return false;
    }

    {
        std::lock_guard lock(mutex_);
        window_ = window;
        width_ = width;
        height_ = height;
        startup_complete_ = false;
        startup_success_ = false;
        running_.store(true);
    }

    {
        std::lock_guard lock(camera_mutex_);
        camera_.reset();
    }

    thread_ = std::thread(&RenderThread::run, this);

    std::unique_lock lock(mutex_);
    condition_.wait(lock, [this] {
        return startup_complete_ || !running_.load();
    });

    return startup_complete_ && startup_success_;
}

void RenderThread::resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    {
        std::lock_guard lock(mutex_);
        width_ = width;
        height_ = height;
    }
    condition_.notify_one();
}

void RenderThread::rotateCamera(
    float startX,
    float startY,
    float endX,
    float endY,
    int width,
    int height) {
    std::lock_guard lock(camera_mutex_);

    if (!camera_controls_enabled_ || !running_.load()) {
        return;
    }

    camera_.rotate(startX, startY, endX, endY, width, height);
}

void RenderThread::panCamera(float deltaX, float deltaY, int width, int height) {
    std::lock_guard lock(camera_mutex_);

    if (!camera_controls_enabled_ || !running_.load()) {
        return;
    }

    camera_.pan(deltaX, deltaY, width, height);
}

void RenderThread::zoomCamera(float scaleFactor) {
    std::lock_guard lock(camera_mutex_);

    if (!camera_controls_enabled_ || !running_.load()) {
        return;
    }

    camera_.zoom(scaleFactor);
}

void RenderThread::setCameraControlsEnabled(bool enabled) {
    std::lock_guard lock(camera_mutex_);
    camera_controls_enabled_ = enabled;
}

void RenderThread::stop() {
    running_.store(false);
    condition_.notify_one();

    if (thread_.joinable()) {
        thread_.join();
    }

    {
        std::lock_guard lock(mutex_);
        window_ = nullptr;
        width_ = 0;
        height_ = 0;
        startup_complete_ = false;
        startup_success_ = false;
    }
}

void RenderThread::run() {
    ANativeWindow* window = nullptr;
    int current_width = 0;
    int current_height = 0;

    {
        std::lock_guard lock(mutex_);
        window = window_;
        current_width = width_;
        current_height = height_;
    }

    EglContext context;
    if (!context.initialize(window)) {
        {
            std::lock_guard lock(mutex_);
            startup_success_ = false;
            startup_complete_ = true;
        }
        running_.store(false);
        condition_.notify_one();
        return;
    }

    GlesRenderer renderer;
    if (!renderer.initialize(context)) {
        {
            std::lock_guard lock(mutex_);
            startup_success_ = false;
            startup_complete_ = true;
        }
        running_.store(false);
        condition_.notify_one();
        context.destroy();
        return;
    }

    {
        std::lock_guard lock(mutex_);
        startup_success_ = true;
        startup_complete_ = true;
    }
    condition_.notify_one();

    while (running_.load()) {
        {
            std::lock_guard lock(mutex_);
            current_width = width_;
            current_height = height_;
        }

        if (current_width > 0 && current_height > 0) {
            glm::mat4 view;
            {
                std::lock_guard lock(camera_mutex_);
                view = camera_.getViewMatrix();
            }
            renderer.render(context, current_width, current_height, view);
        }

        std::unique_lock lock(mutex_);
        condition_.wait_for(lock, std::chrono::milliseconds(16), [this] {
            return !running_.load();
        });
    }

    renderer.destroy();
    context.destroy();
}

} // namespace meshmedic::rendering
