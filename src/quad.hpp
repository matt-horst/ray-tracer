#pragma once

#include "bbox.hpp"
#include "hittable.hpp"
#include "interval.hpp"
#include "material.hpp"
#include <memory>

class Quad : public Hittable {
public:
    Quad(const Point3<double> &origin, const Vec3<double> u, const Vec3<double> &v, std::shared_ptr<Material> mat)
        : origin_(origin), u_(u), v_(v), mat_(mat)
    {
        const auto n = cross(u_, v_);

        normal_ = normalize(n);
        D_ = dot(normal_, Vec3<double>(origin_));
        w_ = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box() {
        const auto bbox_diagonal_1 = BBox3(origin_, origin_ + u_ + v_);
        const auto bbox_diagonal_2 = BBox3(origin_ + u_, origin_ + v_);
        bbox_ = BBox3(bbox_diagonal_1, bbox_diagonal_2);
    }

    BBox3 bounding_box() const override { return bbox_; }

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord &rec) const override {
        const auto denom = dot(normal_, ray.direction());
        
        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8) {
            return false;
        }

        const auto t = (D_ - dot(normal_, Vec3<double>(ray.origin()))) / denom;
        if (!ray_t.contains(t)) {
            return false;
        }

        const auto intersection = ray.at(t);
        const Vec3<double> p = intersection - origin_;
        const auto alpha = dot(w_, cross(p, v_));
        const auto beta = dot(w_, cross(u_, p));

        if (!is_interior(alpha, beta)) {
            return false;
        }

        rec.u = alpha;
        rec.v = beta;
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat_;
        rec.set_face_normal(ray, normal_);

        return true;
    }

    virtual bool is_interior(double a, double b) const {
        Interval interval(0, 1);

        if (!interval.contains(a) || !interval.contains(b)) return false;

        return true;
    }

private:
    Point3<double> origin_;
    Vec3<double> u_;
    Vec3<double> v_;
    std::shared_ptr<Material> mat_;
    BBox3 bbox_;
    Vec3<double> normal_;
    double D_;
    Vec3<double> w_;

    friend struct YAML::convert<std::shared_ptr<Quad>>;
};
