#include "../headers/vectorops.h"

template <>
template <> vector2Float :: vector(const vector2Int& v) :
                 x(v.x),
                 y(v.y) {}

template <>
template <> vector2Int :: vector(const vector2Float& v) :
                 x(int(v.x + .5f)),
                 y(int(v.y + .5f)) {}

template <> 
template <> vector3Float :: vector(const vector3Int& v) :
                 x(v.x),
                 y(v.y),
                 z(v.z) {}

template <> 
template <> vector3Int :: vector(const vector3Float& v) :
                 x(int(v.x + .5f)),
                 y(int(v.y + .5f)),
                 z(int(v.z + .5f)) {}


template <>
template <> vector4Float :: vector(const vector4Int& v) :
                 x(v.x),
                 y(v.y),
                 z(v.z),
                 w(v.w) {}

template <> 
template <> vector4Int :: vector(const vector4Float& v) :
                 x(int(v.x + .5f)),
                 y(int(v.y + .5f)),
                 z(int(v.z + .5f)),
                 w(int(v.w + .5f)) {}