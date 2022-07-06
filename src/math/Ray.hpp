#pragma once

#include "Plane.hpp"

#include <EssaUtil/Vector3.hpp>
#include <optional>

namespace Math {

class Ray {
public:
    Ray(Vector3 start, Vector3 end)
        : m_start(start)
        , m_end(end) { }

    Vector3 start() const { return m_start; }
    Vector3 end() const { return m_end; }

    double distance_to_point(Vector3) const;
    std::optional<Vector3> intersection_with_plane(Plane const& plane) const;

private:
    Vector3 m_start;
    Vector3 m_end;
};

}
