#pragma once

#include "vecdefs.h"

template<size_t SIZE, typename T>
vector<SIZE, T> operator+(vector<SIZE, T> l, const vector<SIZE, T>& r) {
    for (size_t i = SIZE; i--;)
        l[i] += r[i];
    return l;
}

template<size_t SIZE, typename T>
vector<SIZE, T> operator-(vector<SIZE, T> l, const vector<SIZE, T>& r) {
    for (size_t i = SIZE; i--;)
        l[i] -= r[i];
    return l;
}

template<size_t SIZE, typename T, typename U>
vector<SIZE, T> operator*(vector<SIZE, T> l, const U& r) {
    for (size_t i = SIZE; i--;)
        l[i] *= r;
    return l;
}

template<size_t SIZE, typename T, typename U>
vector<SIZE, T> operator/(vector<SIZE, T> l, const U& r) {
    for (size_t i = SIZE; i--;)
        l[i] /= r;
    return l;
}

template<size_t LEN, size_t SIZE, typename T>
vector<LEN, T> project(const vector<SIZE, T>& v) {
    vector<LEN, T> tmp;
    for (size_t i = LEN; i--;)
        tmp[i] = v[i];
    return tmp;
}

template<size_t LEN, size_t SIZE, typename T>
vector<LEN, T> complement(const vector<SIZE, T>& v, T a = 1) {
    vector<LEN, T> tmp;
    for (size_t i = LEN; i--;) {
        if (i >= SIZE) {
            tmp[i] = a;
        } else {
            tmp[i] = v[i];
        }
    }
    return tmp;
}


template<size_t SIZE, typename T>
vector<SIZE, T> operator*(const vector<SIZE, T>& l, const vector<SIZE, T>& r) {
    vector<SIZE, T> tmp;
    for (size_t i = 0; i < SIZE; ++i)
        tmp[i] = l[i] * r[i];
    return tmp;
}

template<size_t SIZE, typename T>
T dot(vector<SIZE, T> l, vector<SIZE, T> r) {
    T tmp = T();
    for (size_t i = SIZE; i--;)
        tmp += l[i] * r[i];
    return tmp;
}

template <typename T>
vector<3, T> cross(vector<3, T> l, vector<3, T> r) {
    return vector<3, T>(l.y * r.z - l.z * r.y,
                        l.z * r.x - l.x * r.z,
                        l.x * r.y - l.y * r.x);
}

typedef vector<2, float> vector2Float;
typedef vector<2, int>   vector2Int;
typedef vector<3, float> vector3Float;
typedef vector<3, int>   vector3Int;
typedef vector<4, float> vector4Float;
typedef vector<4, int>   vector4Int;