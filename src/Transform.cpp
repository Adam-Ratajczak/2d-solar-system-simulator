#include "Transform.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
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

Matrix4x4d rotation_around_y(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return { { { cos, 0, sin, 0 },
        { 0, 1, 0, 0 },
        { -sin, 0, cos, 0 },
        { 0, 0, 0, 1 } } };
}

Matrix4x4d rotation_around_z(double angle) {
    // https://ksuweb.kennesaw.edu/~plaval/math4490/rotgen.pdf
    double sin {}, cos {};
    sincos(angle, &sin, &cos);
    return { { { cos, -sin, 0, 0 },
        { sin, cos, 0, 0 },
        { 0, 0, 1, 0 },
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

Vector3 project(Vector3 p) {
    GLint viewport[4] {-1,-1,1,1};
    GLdouble modelview[16] {};
    GLdouble projection[16] {};
    GLdouble winX {}, winY {}, winZ {}; // 2D point

    // get the matrices
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    //glGetIntegerv(GL_VIEWPORT, viewport);

    int res = gluProject(p.x, p.y, p.z, modelview, projection, viewport, &winX, &winY, &winZ);
    return { winX, winY, winZ };
}

}
