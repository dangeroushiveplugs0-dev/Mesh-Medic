#include "RenderThread.h"

#include <chrono>

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
    const bool initialized = context.initialize(window);

    {
        std::lock_guard lock(mutex_);
        startup_success_ = initialized;
        startup_complete_ = true;
    }
    condition_.notify_one();

    if (!initialized) {
        running_.store(false);
        return;
    }

    GlesRenderer renderer;

    while (running_.load()) {
        {
            std::lock_guard lock(mutex_);
            current_width = width_;
            current_height = height_;
        }

        if (current_width > 0 && current_height > 0) {
            renderer.render(context, current_width, current_height);
        }

        std::unique_lock lock(mutex_);
        condition_.wait_for(lock, std::chrono::milliseconds(16), [this] {
            return !running_.load();
        });
    }

    context.destroy();
}

} // namespace meshmedic::rendering
