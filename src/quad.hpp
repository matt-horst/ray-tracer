#pragma once

#include "bbox.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
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
        Interval interval(0.0, 1.0);

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

class Box : public Hittable {
public:
    Box(const Point3<double> &a, const Point3<double> &b, std::shared_ptr<Material> mat) : a_(a), b_(b), mat_(mat), sides_() {
        const auto min = Point3<double>(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
        const auto max = Point3<double>(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

        const auto dx = Vec3<double>(max.x() - min.x(), 0, 0);
        const auto dy = Vec3<double>(0, max.y() - min.y(), 0);
        const auto dz = Vec3<double>(0, 0, max.z() - min.z());

        sides_.add(std::make_shared<Quad>(Point3<double>(min.x(), min.y(), max.z()), dx, dy, mat));
        sides_.add(std::make_shared<Quad>(Point3<double>(max.x(), min.y(), max.z()), -dz, dy, mat));
        sides_.add(std::make_shared<Quad>(Point3<double>(max.x(), min.y(), min.z()), -dx, dy, mat));
        sides_.add(std::make_shared<Quad>(Point3<double>(min.x(), min.y(), min.z()), dz, dy, mat));
        sides_.add(std::make_shared<Quad>(Point3<double>(min.x(), max.y(), max.z()), dx, -dz, mat));
        sides_.add(std::make_shared<Quad>(Point3<double>(min.x(), max.y(), min.z()), dx, dz, mat));
    }

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord &rec) const override {
        return sides_.hit(ray, ray_t, rec);
    }
    
    BBox3 bounding_box() const override { return sides_.bounding_box(); }

private:
    Point3<double> a_, b_;
    std::shared_ptr<Material> mat_;
    HittableList sides_;

    friend struct YAML::convert<std::shared_ptr<Box>>;
};
