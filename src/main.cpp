#include <memory>
#include <optional>
#include <unistd.h>
#include <cassert>
#include <thread>
#include "camera.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "render.hpp"
#include "util.hpp"
#include "bvh.hpp"

int main(void) {
    CameraParams params;
    params.image_width = 400;;
    params.samples_per_pixel = 100;
    params.max_depth = 50;
    params.lookfrom = Point3<double>(13, 2, 3);
    params.lookat = Point3<double>(0, 1, 0);
    params.vfov = 20.0;
    params.defocus_angle = 0.6;
    params.focus_dist = 10.0;
    const Camera cam(params);

    const auto material_ground = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    const auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    const auto material_left = std::make_shared<Dielectric>(1.5);
    const auto material_bubble = std::make_shared<Dielectric>(1.0 / 1.5);
    const auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    HittableList world;
    world.add(std::make_shared<Sphere>(Point3<double>(0.0, -1000.0, 0.0), 1000, material_ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const auto choose_mat = random_double();
            const Point3<double> center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point3<double>(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;
                std::optional<Point3<double>> center_2 = std::nullopt;

                if (choose_mat < 0.8) {
                    // Diffuse
                    const auto albedo = Color::random() * Color::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    center_2 = std::optional(center + Vec3<double>(0, random_double(0, 0.5), 0));
                } else if (choose_mat < 0.95) {
                    // Metal
                    const auto albedo = Color::random(0.5, 1);
                    const auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                } else {
                    // Glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                }

                if (center_2) {
                    world.add(std::make_shared<Sphere>(center, center_2.value(), 0.2, sphere_material));
                } else {
                    world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    const auto material_1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3<double>(0, 1, 0), 1.0, material_1));

    const auto material_2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3<double>(-4, 1, 0), 1.0, material_2));

    const auto material_3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3<double>(4, 1, 0), 1.0, material_3));

    world = HittableList(std::make_shared<BVHNode>(world));

    render(cam, world, std::thread::hardware_concurrency());
}
