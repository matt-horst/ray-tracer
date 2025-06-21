#pragma once
#include <cstdint>
#include <stdint.h>
#include <cmath>
#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "image.hpp"

class Camera {
public:
    Camera(Point3<double> center,
            double defocus_angle,
            Vec3<double> pixel_delta_u,
            Vec3<double> pixel_delta_v,
            Vec3<double> pixel00_loc,
            Vec3<double> defocus_disk_u,
            Vec3<double> defocus_disk_v) : 
        center_(center),
        defocus_angle_(defocus_angle),
        pixel_delta_u_(pixel_delta_u),
        pixel_delta_v_(pixel_delta_v),
        pixel00_loc_(pixel00_loc),
        defocus_disk_u_(defocus_disk_u),
        defocus_disk_v_(defocus_disk_v)
    {}

    Ray<double> cast_ray_at_pixel_loc(size_t row, size_t col) const {
        const auto offset = sample_square();
        const Point3<double> pixel_center = pixel00_loc_ + ((row + offset.x()) * pixel_delta_v_) + ((col + offset.y()) * pixel_delta_u_);
        const auto ray_origin = (defocus_angle_ <= 0) ? center_ : defocus_disk_sample();
        const Vec3<double> ray_direction = pixel_center - ray_origin;
        const auto ray_time = random_double();

        return Ray<double>(ray_origin, ray_direction, ray_time);
    }

private:
    Point3<double> center_;
    double defocus_angle_;
    Vec3<double> pixel_delta_u_, pixel_delta_v_;
    Vec3<double> pixel00_loc_;
    Vec3<double> defocus_disk_u_, defocus_disk_v_;

    Vec3<double> sample_square() const {
        return Vec3<double>(0.5 - random_double(), 0.5 - random_double(), 0.0);
    }

    Point3<double> defocus_disk_sample() const {
        const auto p = random_on_unit_disk();
        return center_ + (p.x() * defocus_disk_u_) + (p.y() * defocus_disk_v_);
    }
};

class CameraBuilder {
public:
    CameraBuilder() {}

    Camera build(const Image &img) const {
        const auto defocus_angle = degrees_to_radians(defocus_angle_);
        const auto theta = degrees_to_radians(vfov_);
        const auto h = std::tan(theta / 2.0);
        const auto viewport_height = 2 * h * focus_dist_;
        const auto viewport_width = viewport_height * static_cast<double>(img.width_) / img.height_;
        const auto w = normalize(lookfrom_ - lookat_);
        const auto u = normalize(cross(vup_, w));
        const auto v = cross(w, u);
        const auto viewport_u = viewport_width * u;
        const auto viewport_v = viewport_height * -v;
        const auto pixel_delta_u = viewport_u / img.width_;
        const auto pixel_delta_v = viewport_v / img.height_;
        const auto viewport_upper_left = lookfrom_ - (focus_dist_ * w) - (viewport_u / 2) - (viewport_v / 2);
        const auto pixel00_loc(viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v));
        const auto defocus_radius = focus_dist_ * std::tan(defocus_angle / 2.0);
        const auto defocus_disk_u = u * defocus_radius;
        const auto defocus_disk_v = v * defocus_radius;

        return Camera(lookfrom_, defocus_angle_, pixel_delta_u, pixel_delta_v, pixel00_loc, defocus_disk_u, defocus_disk_v);
    }

    Point3<double> lookfrom_ = Point3<double>();
    Point3<double> lookat_ = Point3<double>(0.0, 0.0, -1.0);
    double vfov_ = 90.0;
    Vec3<double> vup_ = Vec3<double>(0.0, 1.0, 0.0);
    double defocus_angle_ = 0.0;
    double focus_dist_ = 10.0;
};
