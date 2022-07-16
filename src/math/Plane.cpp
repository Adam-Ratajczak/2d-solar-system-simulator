#include "Plane.hpp"
#include <iomanip>

namespace Math {

Plane::Plane(Util::Vector3d A, Util::Vector3d B, Util::Vector3d C) {
    // https://keisan.casio.com/exec/system/1223596129
    m_a = (B.y() - A.y()) * (C.z() - A.z()) - (C.y() - A.y()) * (B.z() - A.z());
    m_b = (B.z() - A.z()) * (C.x() - A.x()) - (C.z() - A.z()) * (B.x() - A.x());
    m_c = (B.x() - A.x()) * (C.y() - A.y()) - (C.x() - A.x()) * (B.y() - A.y());
    m_d = -(m_a * A.x() + m_b * A.y() + m_c * A.z());
}

std::ostream& operator<<(std::ostream& out, Math::Plane const& plane) {
    return out << std::fixed << std::setprecision(10) << plane.a() << "x + " << plane.b() << "y + " << plane.c() << "z + " << plane.d() << " = 0";
}

Util::Vector3d Plane::normal() const {
    return Util::Vector3d(m_a, m_b, m_c).normalized();
}

Util::Vector3d Plane::point() const {
    Util::Vector3d plane_point;
    if (m_c == 0)
        return { -m_d / m_a, 0, 0 };
    return { 0, 0, -m_d / m_c };
}

Plane Plane::transformed(Util::Matrix4x4d const& matrix) const {
    // https://stackoverflow.com/questions/7685495/transforming-a-3d-plane-using-a-4x4-matrix
    auto normal = Util::Vector4d { this->normal(), 0 };
    auto point = Util::Vector4d { this->point() };
    // std::cout << normal << "," << point << std::endl;

    auto transformed_point = matrix * point;
    transformed_point /= transformed_point.w();
    auto transformed_normal = matrix.inverted().transposed() * normal;
    transformed_normal /= transformed_normal.w();

    double d = transformed_normal.dot(transformed_point);
    return Plane {
        transformed_normal.x(),
        transformed_normal.y(),
        transformed_normal.z(),
        -d
    };
}

}
