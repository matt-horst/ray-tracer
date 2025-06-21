#pragma once
#include "bbox.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "yaml-cpp/yaml.h"
#include <memory>

class Sphere : public Hittable {
public:
    // Static Sphere
    Sphere(const Point3<double> &origin, double radius, std::shared_ptr<Material> mat) : origin_(origin, Vec3<double>()), radius_(std::fmax(radius, 0.0)), mat_(mat) {
        auto rvec = Vec3<double>(radius_, radius_, radius_);
        bbox_ = BBox3(origin_.origin() - rvec, origin_.origin() + rvec);
    }

    // Moving Sphere
    Sphere(const Point3<double> &origin_1, const Point3<double> &origin_2, double radius, std::shared_ptr<Material> mat) : origin_(origin_1, origin_2 - origin_1), radius_(std::fmax(radius, 0.0)), mat_(mat) {
        auto rvec = Vec3<double>(radius_, radius_, radius_);
        const auto origin_at_0 = origin_.at(0);
        const auto bbox_at_0 = BBox3(origin_at_0 - rvec, origin_at_0 + rvec);
        const auto origin_at_1 = origin_.at(1);
        const auto bbox_at_1 = BBox3(origin_at_1 - rvec, origin_at_1 + rvec);

        bbox_ = BBox3(bbox_at_0, bbox_at_1);
    }

    Sphere(const Ray<double> &origin, double radius, std::shared_ptr<Material> mat) : origin_(origin), radius_(std::fmax(radius, 0.0)), mat_(mat) {
        auto rvec = Vec3<double>(radius_, radius_, radius_);
        const auto origin_at_0 = origin_.at(0);
        const auto bbox_at_0 = BBox3(origin_at_0 - rvec, origin_at_0 + rvec);
        const auto origin_at_1 = origin_.at(1);
        const auto bbox_at_1 = BBox3(origin_at_1 - rvec, origin_at_1 + rvec);

        bbox_ = BBox3(bbox_at_0, bbox_at_1);
    }

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const override {
        const Point3<double> current_origin = origin_.at(ray.time());
        const auto diff = current_origin - ray.origin();
        const auto a = ray.direction().length_sqr();
        const auto h = dot(ray.direction(), diff);
        const auto c = diff.length_sqr() - radius_ * radius_;
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
        Vec3<double> outward_normal = (rec.p - current_origin) / radius_;
        rec.set_face_normal(ray, outward_normal);
        get_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat_;

        return true;
    }

    BBox3 bounding_box() const override { return bbox_; }

private:
    Ray<double> origin_;
    double radius_;
    std::shared_ptr<Material> mat_;
    BBox3 bbox_;

    static void get_uv(const Point3<double> &p, double &u, double &v) {
        const auto theta = std::acos(-p.y());
        const auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

    friend struct YAML::convert<std::shared_ptr<Sphere>>;
};
