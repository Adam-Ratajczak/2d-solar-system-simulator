#pragma once

#include "Matrix.hpp"

namespace Transform {

Matrix4x4d translation(Vector3);
Matrix4x4d rotation_around_x(double angle);
Matrix4x4d scale(Vector3 axis);

}
