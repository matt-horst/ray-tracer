#pragma once
#include "ray.hpp"
#include "hittable.hpp"
#include <memory>

class Sphere : public Hittable {
public:
    // Static Sphere
    Sphere(const Point3<double> &origin_, double radius_, std::shared_ptr<Material> mat) : origin(origin_, Vec3<double>()), radius(std::fmax(radius_, 0.0)), mat(mat) {}

    // Moving Sphere
    Sphere(const Point3<double> &origin_1, const Point3<double> &origin_2, double radius_, std::shared_ptr<Material> mat) : origin(origin_1, origin_2 - origin_1), radius(std::fmax(radius_, 0.0)), mat(mat) {}

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const override {
        const Point3<double> current_origin = origin.at(ray.time());
        const auto diff = current_origin - ray.origin();
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
        Vec3<double> outward_normal = (rec.p - current_origin) / radius;
        rec.set_face_normal(ray, outward_normal);
        rec.mat = mat;

        return true;
    }

private:
    Ray<double> origin;
    double radius;
    std::shared_ptr<Material> mat;
};
