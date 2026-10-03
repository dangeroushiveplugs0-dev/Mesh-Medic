#pragma once

#include <array>
#include <cmath>

namespace meshmedic::rendering {

struct Mat4 {
    std::array<float, 16> value{};

    static Mat4 identity() {
        Mat4 result{};
        result.value = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        return result;
    }

    static Mat4 perspective(float fov_radians, float aspect, float near_plane, float far_plane) {
        Mat4 result{};
        const float f = 1.0f / std::tan(fov_radians * 0.5f);
        result.value = {
            f / aspect, 0.0f, 0.0f, 0.0f,
            0.0f, f, 0.0f, 0.0f,
            0.0f, 0.0f, (far_plane + near_plane) / (near_plane - far_plane), -1.0f,
            0.0f, 0.0f, (2.0f * far_plane * near_plane) / (near_plane - far_plane), 0.0f
        };
        return result;
    }

    static Mat4 rotation_y(float radians) {
        Mat4 result = identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        result.value[0] = c;
        result.value[2] = -s;
        result.value[8] = s;
        result.value[10] = c;
        return result;
    }

    static Mat4 rotation_x(float radians) {
        Mat4 result = identity();
        const float c = std::cos(radians);
        const float s = std::sin(radians);
        result.value[5] = c;
        result.value[6] = s;
        result.value[9] = -s;
        result.value[10] = c;
        return result;
    }

    static Mat4 translation(float x, float y, float z) {
        Mat4 result = identity();
        result.value[12] = x;
        result.value[13] = y;
        result.value[14] = z;
        return result;
    }
};

inline Mat4 multiply(const Mat4& a, const Mat4& b) {
    Mat4 result{};
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int i = 0; i < 4; ++i) {
                sum += a.value[i * 4 + row] * b.value[column * 4 + i];
            }
            result.value[column * 4 + row] = sum;
        }
    }
    return result;
}

} // namespace meshmedic::rendering
