#pragma once

#include "matdefs.h"

template<size_t M, size_t N, typename T>
vector<M, T> Matrix<M, N, T>::col(const size_t j) const {
    assert(j < N);
    vector<M, T> tmp;
    for (size_t i = M; i--;)
        tmp[i] = matrix[i][j];
    return tmp;
}

template<size_t M, size_t N, typename T>
void Matrix<M, N, T>::setCol(size_t j, vector<M, T> v) {
    assert(j < N);
    for (size_t i = M; i--;)
        matrix[i][j] = v[i];
}

template<size_t M, size_t N, typename T>
vector<N, T>& Matrix<M, N, T>::operator[] (const size_t j) {
    assert(j < M);
    return matrix[j];
}

template<size_t M, size_t N, typename T>
const vector<N, T>& Matrix<M, N, T>::operator[] (const size_t j) const {
    assert(j < M);
    return matrix[j];
}

template<size_t M, size_t N, typename T>
vector<M, T> operator*(const Matrix<M, N, T>& l,
                       const vector<N, T>& r) {
    vector<M, T> tmp;
    for (size_t i = M; i--;)
        tmp[i] = dot(l[i], r);
    return tmp;
}

template<size_t M, size_t N, size_t K, typename T>
Matrix<M, K, T> operator*(const Matrix<M, N, T>& l,
                          const Matrix<N, K, T>& r) {
    Matrix<M, K, T> tmp;
    for (size_t i = M; i--;) {
        for (size_t j = K; j--;) {
            tmp[i][j] = dot(l[i], r.col(j));
        }
    }
    return tmp;
}

template<size_t M, size_t N, typename T>
Matrix<N, M, T> operator/(Matrix<M, N, T> l,
                          const T& r) {
    for (size_t i = M; i--;) {
        l[i] = l[i] / r;
    }
    return l;
}

typedef Matrix<4, 4, float> Matrix4x4;
typedef Matrix<3, 3, float> Matrix3x3;

Matrix4x4 Perspective(double near, double far);
Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far);
Matrix4x4 Projection(double fov, double ratio, double near, double far);

Matrix4x4 Viewport(unsigned width, unsigned height);
vector3Float Barycentric(vector2Float A, vector2Float B, vector2Float C, vector2Float P);
Matrix4x4 View(vector3Float pov, vector3Float center, vector3Float top);
