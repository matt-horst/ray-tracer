#pragma once
#include "interval.hpp"
#include "ray.hpp"

class BBox3 {
public:
    Interval x, y, z;

    BBox3() {}
    BBox3(const Interval &x, const Interval &y, const Interval &z) :x(x), y(y), z(z) {
        pad_to_minimums();
    }
    BBox3(const Point3<double> &a, const Point3<double> &b) : 
        x((a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0])),
        y((a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1])),
        z((a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]))
    { 
        pad_to_minimums();
    }

    BBox3(const BBox3 &a, const BBox3 &b) : x(a.x, b.x), y(a.y, b.y), z(a.z, b.z) {}

    const Interval& axis_interval(int n) const {
        switch (n) {
            case 1: return y;
            case 2: return z;
            default: return x;
        }
    }

    bool hit(const Ray<double> ray, Interval ray_t) const {
        const Point3<double> &ray_origin = ray.origin();
        const Vec3<double> &ray_dir = ray.direction();

        for (int axis = 0; axis < 3; axis++) {
            const Interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            const auto t0 = (ax.min - ray_origin[axis]) * adinv;
            const auto t1 = (ax.max - ray_origin[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min) {
                return false;
            }
        }

        return true;
    }

    int32_t longest_axis() const {
        if (x.span() > y.span()) {
            return x.span() > z.span() ? 0 : 2;
        } else {
            return y.span() > z.span() ? 1 : 2;
        }
    }

    static const BBox3 empty, universe;

private:
    void pad_to_minimums() {
        double delta = 1e-4;
        if (x.span() < delta) x = x.expand(delta);
        if (y.span() < delta) y = y.expand(delta);
        if (z.span() < delta) z = z.expand(delta);
    }
};

inline BBox3 operator+(const BBox3 &bbox, const Vec3<double> &offset) {
    return BBox3(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

inline BBox3 operator+(const Vec3<double> &offset, const BBox3 &bbox) {
    return bbox + offset;
}
