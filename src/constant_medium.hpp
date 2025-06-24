#pragma once

#include "bbox.hpp"
#include "hittable.hpp"
#include "interval.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "util.hpp"

class ConstantMedium : public Hittable {
public:
    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> texture) : boundary_(boundary), neg_inv_density_(-1 / density), phase_function_(std::make_shared<Isotropic>(texture)) {}
    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const Color & color) : boundary_(boundary), neg_inv_density_(-1 / density), phase_function_(std::make_shared<Isotropic>(color)) {}
    ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Material> mat_) : boundary_(boundary), neg_inv_density_(-1 / density), phase_function_(mat_) {}

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord &rec) const override {
        HitRecord rec_1, rec_2;

        if (!boundary_->hit(ray, Interval::universe, rec_1)) return false;

        if (!boundary_->hit(ray, Interval(rec_1.t + 1e-4, infinity), rec_2)) return false;

        if (rec_1.t < ray_t.min) rec_1.t = ray_t.min;
        if (rec_2.t > ray_t.max) rec_2.t = ray_t.max;

        if (rec_1.t >= rec_2.t) return false;

        if (rec_1.t < 0) rec_1.t = 0;

        const auto ray_length = ray.direction().length();
        const auto distance_inside_boundary = (rec_2.t - rec_1.t) * ray_length;
        const auto hit_distance = neg_inv_density_ * std::log(random_double());

        if (hit_distance > distance_inside_boundary) return false;

        rec.t = rec_1.t + hit_distance / ray_length;
        rec.p = ray.at(rec.t);

        rec.normal = Vec3<double>(1, 0, 0);     // Arbitrary
        rec.front_face = true;                  // Arbitrary
        rec.mat = phase_function_;

        return true;
    }

    BBox3 bounding_box() const override { return boundary_->bounding_box(); }

    inline static const std::string NAME = "constant-medium";

private:
    std::shared_ptr<Hittable> boundary_;
    double neg_inv_density_;
    std::shared_ptr<Material> phase_function_;

    friend struct YAML::convert<std::shared_ptr<ConstantMedium>>;
};
