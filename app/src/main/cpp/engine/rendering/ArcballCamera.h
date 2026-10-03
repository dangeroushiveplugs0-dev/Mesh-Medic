#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace meshmedic::rendering {

class ArcballCamera {
public:
    ArcballCamera();

    void rotate(
        float startX,
        float startY,
        float endX,
        float endY,
        int width,
        int height);

    void pan(float deltaX, float deltaY, int width, int height);
    void zoom(float scaleFactor);

    glm::mat4 getViewMatrix() const;

    void reset();

private:
    glm::vec3 forwardVector() const;
    glm::vec3 upVector() const;
    void rebuildOrientation();

    glm::vec3 position_{0.0f, 0.0f, 5.0f};
    glm::quat orientation_{1.0f, 0.0f, 0.0f, 0.0f};

    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float zoomDistance_ = 5.0f;

    static constexpr float kMinZoomDistance = 0.25f;
    static constexpr float kMaxZoomDistance = 100.0f;
    static constexpr float kMaxPitch = 1.55334306f; // 89 degrees
    static constexpr float kRotationSensitivity = 0.25f;
};

} // namespace meshmedic::rendering
