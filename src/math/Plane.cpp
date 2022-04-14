#include "Plane.hpp"
#include <iomanip>

namespace Math {

Plane::Plane(Vector3 A, Vector3 B, Vector3 C) {
    // https://keisan.casio.com/exec/system/1223596129
    m_a = (B.y - A.y) * (C.z - A.z) - (C.y - A.y) * (B.z - A.z);
    m_b = (B.z - A.z) * (C.x - A.x) - (C.z - A.z) * (B.x - A.x);
    m_c = (B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y);
    m_d = -(m_a * A.x + m_b * A.y + m_c * A.z);
}

std::ostream& operator<<(std::ostream& out, Math::Plane const& plane) {
    return out << std::fixed << std::setprecision(10) << plane.a() << "x + " << plane.b() << "y + " << plane.c() << "z + " << plane.d() << " = 0";
}

Vector3 Plane::normal() const {
    return Vector3(m_a, m_b, m_c).normalized();
}

Vector3 Plane::point() const {
    Vector3 plane_point;
    if (m_c == 0)
        return { -m_d / m_a, 0, 0 };
    return { 0, 0, -m_d / m_c };
}

Plane Plane::transformed(Matrix4x4d const& matrix) const {
    // https://stackoverflow.com/questions/7685495/transforming-a-3d-plane-using-a-4x4-matrix
    auto normal = this->normal();
    auto point = this->point();

    auto transformed_point = matrix * point;
    normal.w = 0;
    auto transformed_normal = matrix.inverted().transposed() * normal;

    double d = -(transformed_normal.x * transformed_point.x
        + transformed_normal.y * transformed_point.y
        + transformed_normal.z * transformed_point.z);
    return Plane {
        transformed_normal.x,
        transformed_normal.y,
        transformed_normal.z,
        d
    };
}

}
