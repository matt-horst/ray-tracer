#pragma once
#include "ray.hpp"
#include "hittable.hpp"

class Sphere : public Hittable {
public:
    Sphere(const Point3<double> &origin_, double radius_) : origin(origin_), radius(std::fmax(radius_, 0.0)) {}
    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const override {
        const auto diff = origin - ray.origin();
        const auto a = ray.direction().length_sqr();
        const auto h = dot(ray.direction(), diff);
        const auto c = diff.length_sqr() - radius * radius;
        const auto discriminant = h * h - a * c;

        if (discriminant < 0) {
            // No hit
            return false;
        } 

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                return false;
            }
        }

        rec.t = root;
        rec.p = ray.at(rec.t);
        Vec3<double> outward_normal = (rec.p - origin) / radius;
        rec.set_face_normal(ray, outward_normal);

        return true;
    }

    Point3<double> origin;
    double radius;
};
