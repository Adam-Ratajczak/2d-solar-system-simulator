#pragma once

#include <EssaGUI/util/Matrix.hpp>
#include <EssaGUI/util/Vector3.hpp>

namespace Math {

// Ax + By + Cz + D = 0
class Plane {
public:
    Plane(double a, double b, double c, double d)
        : m_a(a)
        , m_b(b)
        , m_c(c)
        , m_d(d) { }

    Plane(Vector3 p1, Vector3 p2, Vector3 p3);

    double a() const { return m_a; }
    double b() const { return m_b; }
    double c() const { return m_c; }
    double d() const { return m_d; }

    Vector3 normal() const;

    // *one of points that is known to be on the plane
    Vector3 point() const;

    Plane transformed(Matrix4x4d const&) const;

private:
    friend std::ostream& operator<<(std::ostream& out, Plane const&);

    double m_a = 0;
    double m_b = 0;
    double m_c = 0;
    double m_d = 0;
};

}