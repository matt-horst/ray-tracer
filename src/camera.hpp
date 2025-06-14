#pragma once
#include <cstdint>
#include <stdint.h>
#include <cmath>
#include "hittable_list.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "material.hpp"

struct CameraParams {
    double aspect_ratio = (16.0 / 9.0);
    int32_t image_width = 400;
    Point3<double> lookfrom = Point3<double>();
    Point3<double> lookat = Point3<double>(0.0, 0.0, -1.0);
    int32_t samples_per_pixel = 10;
    int32_t max_depth = 10;
    double vfov = 90.0;
    Vec3<double> vup = Vec3<double>(0.0, 1.0, 0.0);
    double defocus_angle = 0.0;
    double focus_dist = 10.0;
};


class Camera {
public:
    Camera(const CameraParams &params = CameraParams()) : 
        aspect_ratio(params.aspect_ratio),
        image_width(params.image_width),
        center(params.lookfrom),
        samples_per_pixel(params.samples_per_pixel),
        max_depth(params.max_depth),
        vfov(params.vfov),
        defocus_angle(degrees_to_radians(params.defocus_angle)),
        focus_dist(params.focus_dist),
        image_height(std::max(static_cast<int32_t>(image_width / aspect_ratio), 1)),
        theta(degrees_to_radians(vfov)),
        h(std::tan(theta / 2.0)),
        viewport_height(2 * h * focus_dist),
        viewport_width(viewport_height * static_cast<double>(image_width) / image_height),
        w(normalize(params.lookfrom - params.lookat)),
        u(normalize(cross(params.vup, w))),
        v(cross(w, u)),
        viewport_u(viewport_width * u),
        viewport_v(viewport_height * -v),
        pixel_delta_u(viewport_u / image_width),
        pixel_delta_v(viewport_v / image_height),
        viewport_upper_left(Vec3<double>(center) - (focus_dist * w) - (viewport_u / 2) - (viewport_v / 2)),
        pixel00_loc(viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v)),
        pixel_color_scale(1.0/samples_per_pixel),
        defocus_radius(focus_dist * std::tan(defocus_angle / 2.0)),
        defocus_disk_u(u * defocus_radius),
        defocus_disk_v(v * defocus_radius)
    {
    }


    Ray<double> cast_ray_at_pixel_loc(size_t row, size_t col) const {
        const auto offset = sample_square();
        const Point3<double> pixel_center = pixel00_loc + ((row + offset.x()) * pixel_delta_v) + ((col + offset.y()) * pixel_delta_u);
        const auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        const Vec3<double> ray_direction = pixel_center - ray_origin;

        return Ray<double>(ray_origin, ray_direction);
    }

private:
    double aspect_ratio;
public:
    int32_t image_width;
private:
    Point3<double> center;
public:
    int32_t samples_per_pixel;
    int32_t max_depth;
private:
    double vfov;
    double defocus_angle, focus_dist;
public:
    int32_t image_height;
private:
    double theta;
    double h;
    double viewport_height;
    double viewport_width;
    Vec3<double> w, u, v;
    Vec3<double> viewport_u;
    Vec3<double> viewport_v;
    Vec3<double> pixel_delta_u;
    Vec3<double> pixel_delta_v;
    Vec3<double> viewport_upper_left;
    Vec3<double> pixel00_loc;
public:
    double pixel_color_scale;
private:
    double defocus_radius;
    Vec3<double> defocus_disk_u, defocus_disk_v;


    Vec3<double> sample_square() const {
        return Vec3<double>(0.5 - random_double(), 0.5 - random_double(), 0.0);
    }

    Point3<double> defocus_disk_sample() const {
        const auto p = random_on_unit_disk();
        return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
    }
};
