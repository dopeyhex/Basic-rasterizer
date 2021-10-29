#pragma once

#include <cassert>
#include <cmath>
#include <vector>
//definitions of vectors

template <size_t SIZE, typename T>
class vector {
private:
    std::vector<T> data;
public:
    vector() {
        data.resize(SIZE);
        for (size_t i = SIZE; i--;)
            data[i] = T();
    }

    T& operator[](const size_t i) {
        assert(i < SIZE);
        return data[i];
    }

    const T& operator[](const size_t i) const {
        assert(i < SIZE);
        return data[i];
    }
};

template <typename T>
class vector<2, T> {
public:
    T x, y;

    vector() : 
        x(T()),
        y(T()) {}

    vector(T X, T Y) : 
        x(X),
        y(Y) {}

    template <class U>
    vector<2, T>(const vector<2, U>& v);

    T& operator[](const size_t i) {
        assert(i < 2);
        switch(i) {
            case 1:
                return y;
                break;
            default:
                return x;
                break;
        }
    }

    const T& operator[](const size_t i) const {
        assert(i < 2);
        switch(i) {
            case 1:
                return y;
                break;
            default:
                return x;
                break;
        }
    }

    float norm() {
        return std::sqrt(x * x + y * y);
    }
};

template <typename T>
class vector<3, T> {
public:
    T x, y, z;

    vector() : 
        x(T()),
        y(T()),
        z(T()) {}

    vector(T X, T Y, T Z) : 
        x(X),
        y(Y),
        z(Z) {}

    template <class U>
    vector<3, T>(const vector<3, U>& v);

    T& operator[](const size_t i) {
        assert(i < 3);
        switch(i) {
            case 1:
                return y;
                break;
            case 2:
                return z;
                break;
            default:
                return x;
                break;
        }
    }

    const T& operator[](const size_t i) const {
        assert(i < 3);
        switch(i) {
            case 1:
                return y;
                break;
            case 2:
                return z;
                break;
            default:
                return x;
                break;
        }
    }

    float norm() {
        return std::sqrt(x * x + y * y + z * z);
    }

    vector<3, T>& normalize() {
        *this = (*this) * (1 / norm());
        return *this;
    }
};

template <typename T>
class vector<4, T> {
public:
    T x, y, z, w;

    vector() : 
        x(T()),
        y(T()),
        z(T()),
        w(T()) {}

    vector(T X, T Y, T Z, T W) : 
        x(X),
        y(Y),
        z(Z),
        w(W) {}

    vector(vector<3, T> v, T W) :
        x(v.x),
        y(v.y),
        z(v.z),
        w(W) {}

    template <class U>
    vector<4, T>(const vector<4, U>& v);

    T& operator[](const size_t i) {
        assert(i < 4);
        switch(i) {
            case 1:
                return y;
                break;
            case 2:
                return z;
                break;
            case 3:
                return w;
                break;
            default:
                return x;
                break;
        }
    }

    const T& operator[](const size_t i) const {
        assert(i < 4);
        switch(i) {
            case 1:
                return y;
                break;
            case 2:
                return z;
                break;
            case 3:
                return w;
                break;
            default:
                return x;
                break;
        }
    }

    float norm() {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    vector<4, T>& normalize() {
        *this = (*this) * (1 / norm());
        return *this;
    }
};
