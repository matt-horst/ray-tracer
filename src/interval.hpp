#pragma once
#include "util.hpp"

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {} // Default is empty

    Interval(double min, double max) : min(min), max(max) {}

    double span() const { return max - min; }

    bool contains(double x) const { return min <= x && x <= max; }

    bool surrounds(double x) const { return min < x && x < max; }

    static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+infinity, - infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);
