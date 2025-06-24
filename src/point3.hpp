#pragma once
#include "vec3.hpp"
#include <cstddef>

template<typename T>
struct Point3 {
    T elem[3];

    Point3() : elem{0, 0, 0} {}
    Point3(const Vec3<T> &v) : elem{v.elem[0], v.elem[1], v.elem[2]} {}
    Point3(T x, T y, T z) : elem{x, y, z} {}

    inline T x() const { return elem[0]; }
    inline T y() const { return elem[1]; }
    inline T z() const { return elem[2]; }

    T operator[](size_t i) const { return elem[i]; }
    T& operator[](size_t i) { return elem[i]; }

    inline Point3<T>& operator*=(T t) {
        elem[0] *= t;
        elem[1] *= t;
        elem[2] *= t;

        return *this;
    }

    inline Point3<T>& operator+=(const Vec3<T> v) {
        elem[0] += v.elem[0];
        elem[1] += v.elem[1];
        elem[2] += v.elem[2];

        return *this;
    }
};

template<typename T>
Point3<T> operator+(const Point3<T> &p, const Vec3<T> &v) { return Vec3<T>(p) + v; }

template<typename T>
Point3<T> operator+(const Vec3<T> &v, const Point3<T> &p) { return p + v; }

template<typename T>
Point3<T> operator-(const Point3<T> &p, const Vec3<T> &v) { return Vec3<T>(p) - v; }

template<typename T>
Point3<T> operator-(const Vec3<T> &v, const Point3<T> &p) { return v - Vec3<T>(p); }

template<typename T>
Point3<T> operator*(const Point3<T> &p, double t) { return Vec3<T>(p) * t;}

template<typename T>
Point3<T> operator*(double t, const Point3<T> &p) { return p * t; }

template<typename U, typename V>
Vec3<decltype(U() - V())> operator-(const Point3<U> &u, const Point3<V> &v) { return Vec3<U>(u) - Vec3<V>(v); }
