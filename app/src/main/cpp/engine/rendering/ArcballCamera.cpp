#include "ArcballCamera.h"

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <cmath>

namespace meshmedic::rendering {

namespace {

constexpr float kPi = 3.14159265358979323846f;

} // namespace

ArcballCamera::ArcballCamera() = default;

void ArcballCamera::rotate(
    float startX,
    float startY,
    float endX,
    float endY,
    int width,
    int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    // Free-look camera: the camera turns in place instead of orbiting a
    // target point. The cube therefore stays fixed in world space while the
    // view direction changes around the camera.
    const float referenceSize = static_cast<float>(std::min(width, height));

    const float deltaX = endX - startX;
    const float deltaY = endY - startY;

    yaw_ -= (deltaX / referenceSize) * kPi * kRotationSensitivity;
    pitch_ += (deltaY / referenceSize) * kPi * kRotationSensitivity;

    pitch_ = glm::clamp(pitch_, -kMaxPitch, kMaxPitch);

    rebuildOrientation();
}

void ArcballCamera::pan(float deltaX, float deltaY, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    const float viewportHeight = static_cast<float>(height);
    const float worldUnitsPerPixel =
        (zoomDistance_ * 2.0f) / viewportHeight;

    const glm::vec3 right =
        glm::normalize(orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::vec3 up =
        glm::normalize(orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));

    position_ +=
        (-right * deltaX + up * deltaY) * worldUnitsPerPixel;
}

void ArcballCamera::zoom(float scaleFactor) {
    if (!std::isfinite(scaleFactor) || scaleFactor <= 0.000001f) {
        return;
    }

    const float oldDistance = zoomDistance_;
    zoomDistance_ = glm::clamp(
        zoomDistance_ / scaleFactor,
        kMinZoomDistance,
        kMaxZoomDistance);

    // Dolly the camera along its current view direction. This is a camera
    // movement, not a change in an orbit radius around the cube.
    position_ += forwardVector() * (oldDistance - zoomDistance_);
}

glm::mat4 ArcballCamera::getViewMatrix() const {
    const glm::vec3 eye = position_;
    const glm::vec3 forward = forwardVector();
    const glm::vec3 up = upVector();

    return glm::lookAtRH(eye, eye + forward, up);
}

void ArcballCamera::reset() {
    position_ = glm::vec3(0.0f, 0.0f, 5.0f);
    yaw_ = 0.0f;
    pitch_ = 0.0f;
    zoomDistance_ = 5.0f;
    rebuildOrientation();
}

void ArcballCamera::rebuildOrientation() {
    const glm::quat yawRotation =
        glm::angleAxis(yaw_, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat pitchRotation =
        glm::angleAxis(pitch_, glm::vec3(1.0f, 0.0f, 0.0f));

    orientation_ = glm::normalize(yawRotation * pitchRotation);
}

glm::vec3 ArcballCamera::forwardVector() const {
    return glm::normalize(
        orientation_ * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 ArcballCamera::upVector() const {
    return glm::normalize(
        orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace meshmedic::rendering
