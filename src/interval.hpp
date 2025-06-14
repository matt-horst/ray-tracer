#pragma once

#include "util.hpp"

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {} // Default is empty

    Interval(double min, double max) : min(min), max(max) {}

    Interval(const Interval &a, const Interval &b) : min(std::min(a.min, b.min)), max(std::max(a.max, b.max)) {}

    double span() const { return max - min; }

    bool contains(double x) const { return min <= x && x <= max; }

    bool surrounds(double x) const { return min < x && x < max; }

    double clamp(double x) const {
        if (x <= min) return min;
        else if (x >= max) return max;

        return x; 
    }

    Interval expand(double delta) {
        const double padding = delta / 2;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;
};

