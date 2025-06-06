#pragma once
#include <cstdint>
#include <stdint.h>
#include <cmath>
#include "hittable_list.hpp"
#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"

struct CameraParams {
    double aspect_ratio = (16.0 / 9.0);
    int32_t image_width = 400;
    double focal_length = 1.0;
    double viewport_height = 2.0;
    Point3<double> center = Vec3<double>();
    int32_t samples_per_pixel = 10;
    int32_t max_depth = 10;
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
        pixel00_loc(viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v)),
        samples_per_pixel(params.samples_per_pixel),
        pixel_color_scale(1.0/samples_per_pixel),
        max_depth(params.max_depth)
    {
    }

    void render(const HittableList& world) const {
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        for (int i = 0; i < image_height; i++) {
            std::clog << "\rScanlines remaining: " << (image_height - i) << ' ' << std::flush;
            for (int j = 0; j < image_width; j++) {
                Color pixel_color(0.0, 0.0, 0.0);
                for (int k = 0; k < samples_per_pixel; k++) {
                    Ray<double> ray = cast_ray_at_pixel_loc(i, j);
                    pixel_color += ray_color(ray, world, 0);
                }
                std::cout << pixel_color * pixel_color_scale << '\n';
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
    int32_t samples_per_pixel;
    double pixel_color_scale;
    int32_t max_depth;

    Ray<double> cast_ray_at_pixel_loc(size_t row, size_t col) const {
        const auto offset = sample_square();
        const Point3<double> pixel_center = pixel00_loc + ((row + offset.x()) * pixel_delta_v) + ((col + offset.y()) * pixel_delta_u);
        const Vec3<double> ray_direction = pixel_center - center;

        return Ray<double>(center, ray_direction);
    }

    Color ray_color(const Ray<double> &ray, const Hittable &world, int32_t depth) const {
        if (depth >= max_depth) return Color();

        HitRecord rec;
        if (world.hit(ray, Interval(0.001, infinity), rec)) {
            Vec3<double> direction = rec.normal + random_unit_vector();
            return 0.5 * (ray_color(Ray(rec.p, direction), world, depth + 1));
        }
        const Vec3<double> unit_direction = normalize(ray.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    Vec3<double> sample_square() const {
        return Vec3<double>(0.5 - random_double(), 0.5 - random_double(), 0.0);
    }
};
