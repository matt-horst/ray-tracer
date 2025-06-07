#pragma once
#include "vec3.hpp"
#include "point3.hpp"

template<typename T>
class Ray {
public:
    Ray() : orig(), dir() {}

    Ray(const Point3<T> &origin, const Vec3<T> &dir) : orig(origin), dir(dir) {}

    const Point3<T>& origin() const { return orig; }
    const Vec3<T>& direction() const { return dir; }

    const Point3<double> at(double t) const { return orig + (dir * t); }

private:
    Point3<T> orig;
    Vec3<T> dir;
};

