#pragma once

#include <EssaUtil/Matrix.hpp>

namespace Transform {

// TODO: Use llgl versions of these
Util::Matrix4x4d translation(Util::Vector3d);
Util::Matrix4x4d rotation_around_x(double angle);
Util::Matrix4x4d rotation_around_y(double angle);
Util::Matrix4x4d rotation_around_z(double angle);
Util::Matrix4x4d scale(Util::Vector3d axis);

}
