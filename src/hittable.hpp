#pragma once
#include <memory>
#include "ray.hpp"
#include "interval.hpp"
#include "bbox.hpp"
#include "util.hpp"

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

class Translate : public Hittable {
public:
    Translate(std::shared_ptr<Hittable> object, const Vec3<double> &offset) : object_(object), offset_(offset), bbox_(object_->bounding_box() + offset_) {}


    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const {
        const Ray offset_ray(ray.origin() - offset_, ray.direction(), ray.time());

        if (!object_->hit(offset_ray, ray_t, rec)) {
            return false;
        }

        rec.p += offset_;

        return true;
    }

    BBox3 bounding_box() const { return bbox_; }

private:
    std::shared_ptr<Hittable> object_;
    Vec3<double> offset_;
    BBox3 bbox_;

    friend struct YAML::convert<std::shared_ptr<Translate>>;
};

class RotateY : public Hittable {
public:
    RotateY(std::shared_ptr<Hittable> object, double degrees) : object_(object), theta_(degrees_to_radians(degrees)), cos_theta_(std::cos(theta_)), sin_theta_(std::sin(theta_)), bbox_(object_->bounding_box()) {
        Point3<double> min(infinity, infinity, infinity);
        Point3<double> max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    const auto x = i * bbox_.x.max + (1 - i) * bbox_.x.min;
                    const auto y = i * bbox_.y.max + (1 - i) * bbox_.y.min;
                    const auto z = i * bbox_.z.max + (1 - i) * bbox_.z.min;

                    const auto new_x = cos_theta_ * x + sin_theta_ * z;
                    const auto new_z = -sin_theta_ * x + cos_theta_ * z;

                    const Vec3<double> tester(new_x, y, new_z);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox_ = BBox3(min, max);
    }

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord& rec) const {
        const auto origin = Point3<double>(
                (cos_theta_ * ray.origin().x()) - (sin_theta_ * ray.origin().z()),
                ray.origin().y(),
                (sin_theta_ * ray.origin().x()) + (cos_theta_ * ray.origin().z())
        );

        const auto direction = Vec3<double>(
                (cos_theta_ * ray.direction().x()) - (sin_theta_ * ray.direction().z()),
                ray.direction().y(),
                (sin_theta_ * ray.direction().x()) + (cos_theta_ * ray.direction().z())
        );

        const Ray rotated_ray(origin, direction, ray.time());

        if (!object_->hit(rotated_ray, ray_t, rec)) {
            return false;
        }

        rec.p = Point3<double>(
                (cos_theta_ * rec.p.x()) + (sin_theta_ * rec.p.z()),
                rec.p.y(),
                (-sin_theta_ * rec.p.x()) + (cos_theta_ * rec.p.z())
        );

        rec.normal = Vec3<double>(
                (cos_theta_ * rec.normal.x()) + (sin_theta_ * rec.normal.z()),
                rec.normal.y(),
                (-sin_theta_ * rec.normal.x()) + (cos_theta_ * rec.normal.z())
        );

        return true;
    }

    BBox3 bounding_box() const { return bbox_; }
private:
    std::shared_ptr<Hittable> object_;
    double theta_, cos_theta_, sin_theta_;
    BBox3 bbox_;

    friend struct YAML::convert<std::shared_ptr<RotateY>>;
};
