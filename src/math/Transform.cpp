#include "Transform.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
#include <math.h>

namespace Transform {

Util::Matrix4x4d translation(Util::Vector3d vec) {
    return {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        vec.x(), vec.y(), vec.z(), 1
    };
}

Util::Matrix4x4d rotation_around_x(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return {
        1, 0, 0, 0,
        0, cos, sin, 0,
        0, -sin, cos, 0,
        0, 0, 0, 1
    };
}

Util::Matrix4x4d rotation_around_y(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return {
        cos, 0, sin, 0,
        0, 1, 0, 0,
        -sin, 0, cos, 0,
        0, 0, 0, 1
    };
}

Util::Matrix4x4d rotation_around_z(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return {
        cos, -sin, 0, 0,
        sin, cos, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

Util::Matrix4x4d scale(Util::Vector3d fac) {
    // clang-format off
    return { 
        fac.x(), 0, 0, 0,
        0, fac.y(), 0, 0,
        0, 0, fac.z(), 0,
        0, 0, 0, 1,
    };
    // clang-format on
}

}
