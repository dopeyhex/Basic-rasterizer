#pragma once

#include <cmath>
#include "vectorops.h"

template<size_t M, size_t N, typename T>
class Matrix;

template<size_t SIZE, typename T>
struct determinant {
    static T det(Matrix<SIZE, SIZE, T> mat) {
        size_t flag = 0;

        for (size_t i = 0; i < SIZE; i++) {
            T tol = 1e-7;
            T maxi = 0.0f;
            size_t imax = i;

            for (size_t j = i; j < SIZE; j++) {
                T tmp = fabs(mat[j][i]);
                if (tmp > maxi) { 
                    maxi = tmp;
                    imax = j;
                }
            }

            if (maxi < tol) return 0;

            //pivoting
            if (i != imax) {
                std::swap(mat[i], mat[imax]);
                flag++;
            }

            //elimination
            for (size_t j = i + 1; j < SIZE; j++) {
                T l = mat[i][j] / mat[i][i];
                for (size_t k = 0; k < SIZE; k++) {
                    mat[j][k] -= l * mat[i][k];
                }
            }
        }

        T det = 1;
        for (size_t i = 0; i < SIZE; i++) {
            det *= mat[i][i];
        }

        if (flag % 2) det *= -1;
        return det;
    }
};

template<size_t M, size_t N, typename T>
class Matrix {
private:
    vector<N, T> matrix[M];
public:
    Matrix() {}

    Matrix<N, M, T> transpose() {
        Matrix<N, M, T> tmp;
        for (size_t i = N; i--;)
            tmp[i] = this->col(i);
        return tmp;
    }

    T det() const {
        T d = determinant<N, T>::det(*this);
        return d;
    }

    Matrix<M - 1, N - 1, T> get_minor(size_t row, size_t col) const {
        Matrix<M - 1, N - 1, T> tmp;
        for (size_t i = M - 1; i--;) {
            for (size_t j = N - 1; j--;) {
                size_t tmpi = 0, tmpj = 0;

                if (i < row) {
                    tmpi = i;
                } else {
                    tmpi = i + 1;
                }

                if (j < col) {
                    tmpj = j;
                } else {
                    tmpj = j + 1;
                }

                tmp[i][j] = matrix[tmpi][tmpj];
            }
        }
        return tmp;
    }

    Matrix<M, N, T> adjugate() const {
        Matrix<M, N, T> tmp;
        for (size_t i = M; i--;) {
            for (size_t j = N; j--;) {
                tmp[i][j] = get_minor(i, j).det() * ((i + j) % 2 ? -1 : 1);
            }
        }
        return tmp;
    }

    Matrix<M, N, T> invertT() {
        Matrix<M, N, T> tmp = adjugate();
        T tmp1 = dot(tmp[0], matrix[0]);
        return tmp / tmp1;
    }

    Matrix<M, N, T> invert() {
        return invertT().transpose();
    }

    static Matrix<M, N, T> identity() {
        Matrix<M, N, T> tmp;
        for (size_t i = M; i--;) {
            for (size_t j = N; j--;) {
                tmp[i][j] = (i == j);
            }
        }
        return tmp;
    }

    vector<M, T> col(const size_t j) const;
    void setCol(size_t j, vector<M, T> v);
    vector<N, T>& operator[] (const size_t j);
    const vector<N, T>& operator[] (const size_t j) const;
};
