#pragma once
#include <cmath>
#include <random>

// Constants
constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) { return degrees * pi / 180.0; }

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline int32_t random_int(int min, int max) {
    return random_double(min, max + 1);
}
