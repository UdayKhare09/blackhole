#pragma once

#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {
    }

    Vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {
    }

    Vec3 operator+(const Vec3 &other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3 operator-(const Vec3 &other) const { return {x - other.x, y - other.y, z - other.z}; }
    Vec3 operator*(const float scalar) const { return {x * scalar, y * scalar, z * scalar}; }

    [[nodiscard]] float length() const { return std::sqrt(x * x + y * y + z * z); }

    [[nodiscard]] Vec3 normalize() const {
        const float len = length();
        return len > 0 ? *this * (1.0f / len) : Vec3();
    }

    [[nodiscard]] float dot(const Vec3 &other) const { return x * other.x + y * other.y + z * other.z; }

    [[nodiscard]] Vec3 cross(const Vec3 &other) const {
        return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x};
    }
};

struct Mat4 {
    float m[16]{};

    Mat4() { identity(); }

    void identity() {
        for (float & i : m) i = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    static Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
        Mat4 result;
        const Vec3 f = (center - eye).normalize();
        const Vec3 s = f.cross(up).normalize();
        const Vec3 u = s.cross(f);

        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;
        result.m[12] = -s.dot(eye);
        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;
        result.m[13] = -u.dot(eye);
        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;
        result.m[14] = f.dot(eye);
        result.m[3] = 0;
        result.m[7] = 0;
        result.m[11] = 0;
        result.m[15] = 1;

        return result;
    }

    [[nodiscard]] Mat4 inverse() const {
        Mat4 inv;

        inv.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
                   m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
        inv.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
                   m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
        inv.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
                   m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
        inv.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
                    m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
        inv.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
                   m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
        inv.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
                   m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
        inv.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
                   m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
        inv.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
                    m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
        inv.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
                   m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
        inv.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
                   m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
        inv.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
                    m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
        inv.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
                    m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
        inv.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
                   m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
        inv.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
                   m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
        inv.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
                    m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
        inv.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
                    m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        float det = m[0] * inv.m[0] + m[1] * inv.m[4] + m[2] * inv.m[8] + m[3] * inv.m[12];
        if (det == 0) return *this;

        det = 1.0f / det;
        for (float & i : inv.m) i *= det;
        return inv;
    }
};

class Camera {
public:
    float azimuth = 0.5f;
    float elevation = 1.5f;
    float radius = 15.0f;
    float minRadius = 4.0f;
    float maxRadius = 50.0f;
    Vec3 target = Vec3(0, 0, 0);
    Vec3 position = Vec3(0, 0, 15);

    void updatePosition() {
        position.x = radius * std::sin(elevation) * std::cos(azimuth);
        position.y = radius * std::cos(elevation);
        position.z = radius * std::sin(elevation) * std::sin(azimuth);
    }

    [[nodiscard]] Mat4 getViewMatrix() const {
        return Mat4::lookAt(position, target, Vec3(0, 1, 0));
    }

    [[nodiscard]] Mat4 getInverseViewMatrix() const {
        return getViewMatrix().inverse();
    }
};
