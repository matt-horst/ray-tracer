#pragma once
#include <cmath>
#include <ostream>
#include <stdint.h>
#include <type_traits>
#include <iostream>
#include "vec3.hpp"

constexpr double max_value = std::nexttoward(256.0, 0.0);

struct Color {
    uint8_t elem[3];

    Color() : elem{0, 0, 0} { }

    template<typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    Color(Integer r, Integer g, Integer b) : elem{
        static_cast<uint8_t>(r),
        static_cast<uint8_t>(g),
        static_cast<uint8_t>(b)
    } {}

    template<typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    Color(Floating r, Floating g, Floating b) : elem{
        static_cast<uint8_t>(max_value * r),
        static_cast<uint8_t>(max_value * g),
        static_cast<uint8_t>(max_value * b)
    } {}

    template<typename Integer, std::enable_if_t<std::is_integral<Integer>::value, bool> = true>
    Color(const Vec3<Integer> &v) : elem{
        static_cast<uint8_t>(v.x()),
        static_cast<uint8_t>(v.y()),
        static_cast<uint8_t>(v.z())
    } {}

    template<typename Floating, std::enable_if_t<std::is_floating_point<Floating>::value, bool> = true>
    Color(const Vec3<Floating> &v) : elem{
        static_cast<uint8_t>(max_value * v.x()),
        static_cast<uint8_t>(max_value * v.y()),
        static_cast<uint8_t>(max_value * v.z())
    } {}

    inline uint8_t r() const { return elem[0]; }
    inline uint8_t g() const { return elem[1]; }
    inline uint8_t b() const { return elem[2]; }
};

std::ostream& operator<<(std::ostream& out, const Color color) {
    return out << static_cast<uint32_t>(color.r()) << ' ' << static_cast<uint32_t>(color.g()) << ' ' << static_cast<uint32_t>(color.b());
}
