#pragma once

#include <android/native_window.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "ArcballCamera.h"
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

    void rotateCamera(float startX, float startY, float endX, float endY, int width, int height);
    void panCamera(float deltaX, float deltaY, int width, int height);
    void zoomCamera(float scaleFactor);
    void setCameraControlsEnabled(bool enabled);

private:
    void run();

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::mutex camera_mutex_;
    std::atomic<bool> running_{false};
    bool startup_complete_ = false;
    bool startup_success_ = false;
    bool camera_controls_enabled_ = true;
    ANativeWindow* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    ArcballCamera camera_;
};

} // namespace meshmedic::rendering
