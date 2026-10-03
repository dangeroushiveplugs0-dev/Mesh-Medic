#include "ArcballCamera.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <cmath>

namespace meshmedic::rendering {

namespace {

constexpr float kEpsilon = 0.000001f;
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

    // Android touch coordinates have their origin at the top-left.
    // Convert to OpenGL-style NDC: X right, Y up, both in [-1, 1].
    const float ndcX = (2.0f * x / safeWidth) - 1.0f;
    const float ndcY = 1.0f - (2.0f * y / safeHeight);

    const glm::vec2 ndc(ndcX, ndcY);
    const float lengthSquared = glm::dot(ndc, ndc);

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

    const glm::vec3 start = getSphereVector(startX, startY, width, height);
    const glm::vec3 end = getSphereVector(endX, endY, width, height);

    const float dotProduct = glm::clamp(glm::dot(start, end), -1.0f, 1.0f);
    const glm::vec3 axisCamera = glm::cross(start, end);
    const float axisLength = glm::length(axisCamera);

    if (dotProduct >= 1.0f - kEpsilon) {
        return;
    }

    float angle = std::acos(dotProduct);

    // The cross product becomes numerically unstable for nearly opposite
    // vectors. Select a deterministic perpendicular axis for that case.
    glm::vec3 safeAxisCamera = axisCamera;
    if (axisLength <= kEpsilon) {
        const glm::vec3 fallback =
            std::abs(start.x) < 0.9f
                ? glm::vec3(1.0f, 0.0f, 0.0f)
                : glm::vec3(0.0f, 1.0f, 0.0f);
        safeAxisCamera = glm::normalize(glm::cross(start, fallback));
        angle = kPi;
    } else {
        safeAxisCamera /= axisLength;
    }

    // The arcball vectors are expressed in camera/screen space. Transform
    // the rotation axis into world space before composing the orbit.
    const glm::vec3 axisWorld = glm::normalize(orientation_ * safeAxisCamera);
    const glm::quat delta = glm::angleAxis(angle, axisWorld);

    orientation_ = glm::normalize(delta * orientation_);
}

void ArcballCamera::pan(float deltaX, float deltaY, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    const float viewportHeight = static_cast<float>(height);
    const float worldUnitsPerPixel = (radius_ * 2.0f) / viewportHeight;

    const glm::vec3 right = glm::normalize(
        orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::vec3 up = glm::normalize(
        orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));

    // Move the camera opposite the finger so the model follows the gesture.
    const glm::vec3 translation =
        (-right * deltaX + up * deltaY) * worldUnitsPerPixel;

    target_ += translation;
}

void ArcballCamera::zoom(float scaleFactor) {
    if (!std::isfinite(scaleFactor) || scaleFactor <= kEpsilon) {
        return;
    }

    radius_ = glm::clamp(radius_ / scaleFactor, kMinRadius, kMaxRadius);
}

glm::mat4 ArcballCamera::getViewMatrix() const {
    const glm::vec3 eye = position();
    glm::vec3 up = upVector();
    const glm::vec3 forward = glm::normalize(target_ - eye);

    // Avoid a degenerate look-at basis when the orbit reaches a pole.
    if (std::abs(glm::dot(forward, up)) > 0.999f) {
        up = glm::normalize(orientation_ * glm::vec3(1.0f, 0.0f, 0.0f));
    }

    return glm::lookAtRH(eye, target_, up);
}

void ArcballCamera::reset() {
    target_ = glm::vec3(0.0f);
    orientation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    radius_ = 5.0f;
}

glm::vec3 ArcballCamera::position() const {
    return target_ + orientation_ * glm::vec3(0.0f, 0.0f, radius_);
}

glm::vec3 ArcballCamera::upVector() const {
    return glm::normalize(
        orientation_ * glm::vec3(0.0f, 1.0f, 0.0f));
}

} // namespace meshmedic::rendering
