#pragma once
#include <memory>
#include "ray.hpp"
#include "interval.hpp"
#include "bbox.hpp"

class Material;

struct HitRecord {
    Point3<double> p;
    Vec3<double> normal;
    double t;
    double u;
    double v;
    bool front_face;
    std::shared_ptr<Material> mat;

    void set_face_normal(const Ray<double>& ray, const Vec3<double>& outward_normal) {
        front_face = dot(ray.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

struct Hittable {
    virtual ~Hittable() = default;
    virtual bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const = 0;
    virtual BBox3 bounding_box() const = 0;
};
