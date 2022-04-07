#include "Transform.hpp"
#include <math.h>

namespace Transform {

Matrix4x4d translation(Vector3 vec) {
    return { { { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { vec.x, vec.y, vec.z, 1 } } };
}

Matrix4x4d rotation_around_x(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return { { { 1, 0, 0, 0 },
        { 0, cos, sin, 0 },
        { 0, -sin, cos, 0 },
        { 0, 0, 0, 1 } } };
}

Matrix4x4d scale(Vector3 fac) {
    return { {
        { fac.x, 0, 0, 0 },
        { 0, fac.y, 0, 0 },
        { 0, 0, fac.z, 0 },
        { 0, 0, 0, 1 },
    } };
}

}
