#pragma once
#include <cmath>
#include <ostream>
#include <stdint.h>
#include <type_traits>
#include <iostream>
#include "vec3.hpp"

constexpr double max_value = std::nexttoward(256.0, 0.0);

struct Color {
    double elem[3];

    Color() : elem{0, 0, 0} { }

    template<typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    Color(Integer r, Integer g, Integer b) : elem{
        r / 256.0,
        g / 256.0,
        b / 256.0
    } {}

    template<typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    Color(Floating r, Floating g, Floating b) : elem{ r, g, b } {}

    template<typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    Color(const Vec3<Integer> &v) : elem{
        v.x() / 256.0,
        v.y() / 256.0,
        v.z() / 256.0
    } {}

    template<typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    Color(const Vec3<Floating> &v) : elem{ v.x(), v.y(), v.z() } {}

    inline uint32_t r_int() const { return max_value * elem[0]; }
    inline uint32_t g_int() const { return max_value * elem[1]; }
    inline uint32_t b_int() const { return max_value * elem[2]; }

    inline double r() const { return elem[0]; }
    inline double g() const { return elem[1]; }
    inline double b() const { return elem[2]; }
};

template<>
Vec3<double>::Vec3(const Color &c) : elem{c.r(), c.g(), c.b()} {}

std::ostream& operator<<(std::ostream& out, const Color color) {
    return out << color.r_int() << ' ' << color.g_int() << ' ' << color.b_int();
}

template<typename T>
Color operator*(Color c, T t) {
    return Color(c.elem[0] * t, c.elem[1] * t, c.elem[2] * t);
}

template<typename T>
Color operator*(T t, Color c) { return c * t; }

Color operator+(Color a, Color b) { return Color(Vec3<double>(a) + Vec3<double>(b)); }

