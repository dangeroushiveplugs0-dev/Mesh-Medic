#pragma once

#include <android/native_window.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace meshmedic {

class FilamentRenderer {
public:
    FilamentRenderer();
    ~FilamentRenderer();

    void setSurface(ANativeWindow* window);
    void clearSurface();
    void resize(int width, int height);

private:
    void renderLoop();

    std::atomic<bool> running_{true};
    std::thread renderThread_;

    std::mutex mutex_;
    std::condition_variable condition_;
    ANativeWindow* pendingWindow_ = nullptr;

    std::atomic<int> width_{1};
    std::atomic<int> height_{1};
};

} // namespace meshmedic
