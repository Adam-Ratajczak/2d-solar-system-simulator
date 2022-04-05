#pragma once

#include <SFML/System/Vector3.hpp>
#include <cmath>
#include <ostream>
#include <GL/gl.h>

class Vector3 {
public:
    double x, y, z;

    Vector3()
        : x(0.)
        , y(0.)
        , z(0.) { }
    Vector3(double _x, double _y, double _z)
        : x(_x)
        , y(_y)
        , z(_z) { }

    double magnitude() const;
    double angle() const;

    void limit(double mag);
    void set_magnitude(double mag);
    void glDraw() const;

    // Vector3 rotate_vector(double theta) const;

    double distance_to(Vector3 const& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    Vector3 normalized() const;

    friend std::ostream& operator<<(std::ostream& out, const Vector3& vec){
        return out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    }
};

inline Vector3 operator+(Vector3 a, Vector3 b) { return Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vector3 operator-(Vector3 a, Vector3 b) { return Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vector3 operator*(Vector3 a, double b) { return Vector3(a.x * b, a.y * b, a.z * b); }
inline Vector3 operator*(double a, Vector3 b) { return Vector3(a * b.x, a * b.y, a * b.z); }
inline Vector3 operator/(Vector3 a, double b) { return Vector3(a.x / b, a.y / b, a.z / b); }
inline Vector3 operator-(Vector3 a) { return Vector3(-a.x, -a.y, a.z); }

inline Vector3& operator+=(Vector3& a, Vector3 b) { return a = a + b; }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return a = a - b; }
inline Vector3& operator*=(Vector3& a, double b) { return a = a * b; }
inline Vector3& operator/=(Vector3& a, double b) { return a = a / b; }

inline bool operator==(Vector3& a, Vector3& b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(Vector3& a, Vector3& b) { return !(a == b); }
inline bool operator==(const Vector3& a, const Vector3& b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(const Vector3& a, const Vector3& b) { return !(a == b); }

inline double Vector3::magnitude() const {
    return std::sqrt(x * x + y * y);
}

inline double Vector3::angle() const {
    return (std::atan2(y, x));
}

// inline Vector3 Vector3::rotate_vector(double theta) const {
//     double t_cos = std::cos(theta), t_sin = std::sin(theta);

//     return Vector3(x * t_cos - y * t_sin, x * t_sin + y * t_cos);
// }

inline void Vector3::limit(double mag) {
    double d = this->magnitude();

    if (d < mag)
        return;

    double theta = this->angle();

    x = std::cos(theta) * mag;
    y = std::sin(theta) * mag;
}

inline void Vector3::set_magnitude(double mag) {
    double theta = this->angle();

    x = std::cos(theta) * mag;
    y = std::sin(theta) * mag;
}

inline Vector3 Vector3::normalized() const {
    return Vector3(x, y, z) / magnitude();
}

inline double get_distance(Vector3 a, Vector3 b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

inline void Vector3::glDraw() const{
    glVertex3f(x, y, z);
}
