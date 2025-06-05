#pragma once
#include <cmath>
#include <cstddef>
#include <iostream>

template <typename T>
struct Vec3 {
    T elem[3];

    Vec3() : elem{0, 0, 0} { }

    Vec3(T x, T y, T z) : elem{x, y, z} { }

    inline T x() const { return elem[0]; }
    inline T y() const { return elem[1]; }
    inline T z() const { return elem[2]; }


    Vec3<T>& operator-=(const Vec3<T> &other) {
        elem[0] -= other.x();
        elem[1] -= other.y();
        elem[2] -= other.z();

        return *this;
    }

    Vec3<T>& operator+=(const Vec3<T> &other) {
        elem[0] += other.x();
        elem[1] += other.y();
        elem[2] += other.z();

        return *this;
    }

    Vec3<T>& operator*=(T t) {
        elem[0] *= t;
        elem[1] *= t;
        elem[2] *= t;

        return *this;
    }

    Vec3<double>& operator/=(double t) {
        return *this *= (1 / t);
    }

    double length_sqr() const {
        return this->x() * this->x() + this->y() * this->y() + this->z() * this->z();
    }

    double length() const {
        return std::sqrt(length_sqr());
    }

    T operator[](std::size_t i) const {
        return elem[i];
    }

    T& operator[](std::size_t i) {
        return elem[i];
    }
};

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Vec3<T>& v) {
    return out << '[' << v.x() << ' ' << v.y() << ' ' << v.z() << ']';
}

template<typename U, typename V>
inline Vec3<decltype(U() - V())> operator-(const Vec3<U> &u, const Vec3<V> &v) {
    return Vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

template<typename U, typename V>
inline Vec3<decltype(U() + V())> operator+(const Vec3<U> &u, const Vec3<V> &v) {
    return Vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}

template<typename V, typename T>
inline Vec3<decltype(V() * T())> operator*(T t, const Vec3<V> &v) {
    return Vec3(v.x() * t, v.y() * t, v.z() * t);
}

template<typename V, typename T>
inline Vec3<decltype(V() * T())> operator*(const Vec3<V> &v, T t) {
    return t * v;
}

template<typename V>
inline Vec3<double> operator/(const Vec3<V> &v, double t) {
    return (1 / t) *  v;
}

template<typename U, typename V>
inline decltype(U() * V()) dot(const Vec3<U> &u, const Vec3<V> &v) {
    return u.x() * v.x() + u.y() * v.y() + u.z() * v.z();
}

template<typename T>
inline bool operator==(const Vec3<T> &u, const Vec3<T> &v) {
    return u.x() == v.x() && u.y() == v.y() && u.z() == v.z();
}

template<typename U, typename V>
inline Vec3<decltype(U() * V())> cross(const Vec3<U> &u, const Vec3<V> &v) {
    return Vec3<decltype(U() * V())>(
            u.y() * v.z() - u.z() * v.y(),
            u.z() * v.x() - u.x() * v.z(),
            u.x() * v.y() - u.y() * v.x());
}
