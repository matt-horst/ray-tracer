#pragma once
#include "vec3.hpp"
#include "point3.hpp"
#include "yaml-cpp/yaml.h"

template<typename T>
class Ray {
public:
    Ray() : origin_(), direction_() {}

    Ray(const Point3<T> &origin, const Vec3<T> &dir, double time) : origin_(origin), direction_(dir), time_(time) {}
    Ray(const Point3<T> &origin, const Vec3<T> &dir) : Ray<T>(origin, dir, 0) {}

    const Point3<T>& origin() const { return origin_; }
    const Vec3<T>& direction() const { return direction_; }

    const Point3<double> at(double t) const { return origin_ + (direction_ * t); }

    double time() const { return time_; }

private:
    Point3<T> origin_;
    Vec3<T> direction_;
    double time_;

    friend struct YAML::convert<Ray<T>>;
};

