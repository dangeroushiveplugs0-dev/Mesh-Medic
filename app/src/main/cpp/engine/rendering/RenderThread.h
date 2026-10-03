#pragma once

#include <android/native_window.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "EglContext.h"
#include "GlesRenderer.h"

namespace meshmedic::rendering {

class RenderThread {
public:
    RenderThread() = default;
    ~RenderThread();

    RenderThread(const RenderThread&) = delete;
    RenderThread& operator=(const RenderThread&) = delete;

    bool start(ANativeWindow* window, int width, int height);
    void resize(int width, int height);
    void stop();

private:
    void run();

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_{false};
    ANativeWindow* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};

} // namespace meshmedic::rendering
