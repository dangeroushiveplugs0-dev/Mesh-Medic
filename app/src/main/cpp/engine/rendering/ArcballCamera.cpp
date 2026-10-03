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

glm::vec3 ArcballCamera::getSphereVector(
    float x,
    float y,
    int width,
    int height) const {
    if (width <= 0 || height <= 0) {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    const float safeWidth = static_cast<float>(width);
    const float safeHeight = static_cast<float>(height);

    const float ndcX = (2.0f * x / safeWidth) - 1.0f;
    const float ndcY = 1.0f - (2.0f * y / safeHeight);

    const float lengthSquared = ndcX * ndcX + ndcY * ndcY;
    if (lengthSquared <= 1.0f) {
        return glm::normalize(glm::vec3(
            ndcX,
            ndcY,
            std::sqrt(std::max(0.0f, 1.0f - lengthSquared))));
    }

    const float length = std::sqrt(lengthSquared);
    return glm::vec3(ndcX / length, ndcY / length, 0.0f);
}

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

    // Use a conventional orbit instead of a free trackball. Horizontal
    // movement yaws around the world's up axis; vertical movement pitches
    // around the camera's local right axis. This keeps the horizon stable
    // and prevents the camera from rolling around a moving pivot.
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
    const float worldUnitsPerPixel = (radius_ * 2.0f) / viewportHeight;

    const glm::vec3 right =
        glm::normalize(orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::vec3 up =
        glm::normalize(orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::vec3 translation =
        (-right * deltaX + up * deltaY) * worldUnitsPerPixel;

    target_ += translation;
}

void ArcballCamera::zoom(float scaleFactor) {
    if (!std::isfinite(scaleFactor) || scaleFactor <= 0.000001f) {
        return;
    }

    radius_ = glm::clamp(radius_ / scaleFactor, kMinRadius, kMaxRadius);
}

glm::mat4 ArcballCamera::getViewMatrix() const {
    const glm::vec3 eye = position();
    glm::vec3 up = upVector();

    const glm::vec3 forward = glm::normalize(target_ - eye);
    if (std::abs(glm::dot(forward, up)) > 0.999f) {
        up = glm::normalize(
            orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    return glm::lookAtRH(eye, target_, up);
}

void ArcballCamera::reset() {
    target_ = glm::vec3(0.0f);
    yaw_ = 0.0f;
    pitch_ = 0.0f;
    radius_ = 5.0f;
    rebuildOrientation();
}

void ArcballCamera::rebuildOrientation() {
    const glm::quat yawRotation =
        glm::angleAxis(yaw_, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat pitchRotation =
        glm::angleAxis(pitch_, glm::vec3(1.0f, 0.0f, 0.0f));

    orientation_ = glm::normalize(yawRotation * pitchRotation);
}

glm::vec3 ArcballCamera::position() const {
    return target_ + orientation_ * glm::vec3(0.0f, 0.0f, radius_);
}

glm::vec3 ArcballCamera::upVector() const {
    return glm::normalize(
        orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace meshmedic::rendering
