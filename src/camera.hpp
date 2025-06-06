#pragma once
#include <stdint.h>
#include <cmath>
#include "hittable_list.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"

struct CameraParams {
    double aspect_ratio = (16.0 / 9.0);
    int32_t image_width = 400;
    double focal_length = 1.0;
    double viewport_height = 2.0;
    Point3<double> center = Vec3<double>();
};


class Camera {
public:
    Camera(const CameraParams &params = CameraParams()) : 
        aspect_ratio(params.aspect_ratio),
        image_width(params.image_width),
        image_height(std::max(static_cast<int32_t>(image_width / aspect_ratio), 1)),
        focal_length(params.focal_length),
        viewport_height(params.viewport_height),
        viewport_width(viewport_height * static_cast<double>(image_width) / image_height),
        center(params.center),
        viewport_u(viewport_width, 0, 0),
        viewport_v(0, -viewport_height, 0),
        pixel_delta_u(viewport_u / image_width),
        pixel_delta_v(viewport_v / image_height),
        viewport_upper_left(Vec3<double>(center) - Vec3<double>(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2),
        pixel00_loc(viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v))
    {
    }

    void render(const HittableList& world) const {
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' ' << std::flush;
            for (int j = 0; j < image_width; j++) {
                Ray<double> ray = cast_ray_at_pixel_loc(i, j);
                Color color = ray_color(ray, world);
                std::cout << color << '\n';
            }
        }

        std::clog << "\nDone!\n";
    }

private:
    double aspect_ratio;
    int32_t image_width;
    int32_t image_height;
    double focal_length;
    double viewport_height;
    double viewport_width;
    Point3<double> center;
    Vec3<double> viewport_u;
    Vec3<double> viewport_v;
    Vec3<double> pixel_delta_u;
    Vec3<double> pixel_delta_v;
    Vec3<double> viewport_upper_left;
    Vec3<double> pixel00_loc;

    Ray<double> cast_ray_at_pixel_loc(size_t row, size_t col) const {
        const Point3<double> pixel_center = pixel00_loc + (row * pixel_delta_v) + (col * pixel_delta_u);
        const Vec3<double> ray_direction = pixel_center - center;

        return Ray<double>(center, ray_direction);
    }

    Color ray_color(const Ray<double> &ray, const Hittable &world) const {
        HitRecord rec;
        if (world.hit(ray, Interval(0.0, infinity), rec)) {
            return 0.5 * (rec.normal + Color(1.0, 1.0, 1.0));
        }
        const Vec3<double> unit_direction = normalize(ray.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }
};
