#include "../headers/matrixops.h"

Matrix4x4 Perspective(double near, double far) {
    Matrix4x4 tmp;
    tmp[0][0] = near;
    tmp[1][1] = near;
    tmp[2][2] = near + far;
    tmp[2][3] = -far * near;
    tmp[3][2] = 1.0f;
    return tmp;
}

Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far) {
    Matrix4x4 tmp = Matrix4x4::identity();
    tmp[0][0] = 2.0f / (right - left);
    tmp[1][1] = 2.0f / (top - bottom);
    tmp[2][2] = 2.0f / (near - far);
    tmp[0][3] = (left + right) / (left - right);
    tmp[1][3] = (bottom + top) / (bottom - top);
    tmp[2][3] = (near + far) / (far - near);
    return tmp;
}

Matrix4x4 Projection(double fov, double ratio, double near, double far) {
    float top = (-near) * tanf(fov / 2);
    float right = top * ratio;
    return Orthographic(-right, right, -top, top, near, far) * Perspective(near, far);
}

Matrix4x4 Viewport(unsigned width, unsigned height) {
    Matrix4x4 tmp = Matrix4x4::identity();
    tmp[0][0] = width / 2.0f;
    tmp[1][1] = height / 2.0f;
    tmp[0][3] = tmp[0][0] - 0.5f;
    tmp[1][3] = tmp[1][1] - 0.5f;
    return tmp;
}

vector3Float Barycentric(vector2Float A, vector2Float B, vector2Float C, vector2Float P) {
    vector3Float v[2];
    for (int i = 0; i < 2; i++) {
        v[i][0] = B[i] - A[i];
        v[i][1] = C[i] - A[i];
        v[i][2] = A[i] - P[i];
    }

    vector3Float u = cross(v[0], v[1]);

    if (fabs(u.z) < 1e-5)
        return vector3Float(-1.0f, 1.0f, 1.0f);
    return vector3Float(1.0f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
}

Matrix4x4 View(vector3Float pov, vector3Float center, vector3Float top) {
    vector3Float z = (pov - center).normalize();
    vector3Float x = cross(top, z).normalize();
    vector3Float y = cross(z, x).normalize();

    Matrix4x4 rotate = Matrix4x4::identity();
    Matrix4x4 translate = Matrix4x4::identity();
    for (int i = 0; i < 3; i++) {
        rotate[0][i] = x[i];
        rotate[1][i] = y[i];
        rotate[2][i] = z[i];
        translate[i][3] = -pov[i];
    }
    return rotate * translate;
}
