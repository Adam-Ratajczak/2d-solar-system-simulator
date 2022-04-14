#include "Ray.hpp"

#include <iostream>

namespace Math {

double Ray::distance_to_point(Vector3 point) const {
    // https://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
    double num = (point - m_start).cross(point - m_end).magnitude();
    double den = (m_end - m_start).magnitude();
    return num / den;
}

std::optional<Vector3> Ray::intersection_with_plane(Plane const& plane) const {
    // https://stackoverflow.com/questions/5666222/3d-line-plane-intersection
    // TODO: Do ray&plane, not line&plane intersection
    Vector3 plane_normal  = plane.normal();
    Vector3 plane_point = plane.point();
    plane_normal = plane_normal.normalized();

    Vector3 line_direction = (m_start - m_end).normalized();
    if (plane_normal.dot(line_direction) == 0)
        return {};

    // std::cout << m_start << ":" << m_end << std::endl;

    double t = (plane_normal.dot(plane_point) - plane_normal.dot(m_start)) / plane_normal.dot(line_direction);
    return m_start + line_direction * t;
}

}
