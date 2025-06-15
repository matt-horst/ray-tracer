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
#include "texture.hpp"
#include "util.hpp"
#include "bvh.hpp"
#include "vec3.hpp"

void bouncing_spheres() {
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

    const auto checker_tex = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    const auto material_ground = std::make_shared<Lambertian>(checker_tex);
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

void checkered_spheres() {
    HittableList scene;

    const auto checker_tex = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    scene.add(std::make_shared<Sphere>(Point3<double>(0, -10, 0), 10, std::make_shared<Lambertian>(checker_tex)));
    scene.add(std::make_shared<Sphere>(Point3<double>(0, 10, 0), 10, std::make_shared<Lambertian>(checker_tex)));

    CameraParams cam_params;
    cam_params.aspect_ratio = 16.0 / 9.0;
    cam_params.image_width = 400;
    cam_params.samples_per_pixel = 100;
    cam_params.max_depth = 50;

    cam_params.vfov = 20;
    cam_params.lookfrom = Point3<double>(13, 2, 3);
    cam_params.lookat = Point3<double>(0, 0, 0);
    cam_params.vup = Vec3<double>(0, 1, 0);

    cam_params.defocus_angle = 0;
    
    const Camera cam(cam_params);

    render(cam, scene, std::thread::hardware_concurrency());
}

void earth() {
    const auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
    const auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    const auto globe = std::make_shared<Sphere>(Point3<double>(0, 0, 0), 2, earth_surface);

    
    CameraParams cam_params;
    cam_params.aspect_ratio = 16.0 / 9.0;
    cam_params.image_width = 400;
    cam_params.samples_per_pixel = 100;
    cam_params.max_depth = 50;

    cam_params.vfov = 20;
    cam_params.lookfrom = Point3<double>(0, 0, 12);
    cam_params.lookat = Point3<double>(0, 0, 0);
    cam_params.vup = Vec3<double>(0, 1, 0);

    cam_params.defocus_angle = 0;
    
    const Camera cam(cam_params);

    render(cam, HittableList(globe), 1);
}


int main(int argc, char *argv[]) {
    int opt = atoi(argv[1]);
    switch (opt) {
        case 1: 
            std::clog << "Running bouncing spheres scene...\n";
            bouncing_spheres();
            break;
        case 2:
            std::clog << "Running checker spheres scene...\n";
            checkered_spheres(); 
            break;
        case 3:
            std::clog << "Running earth scene...\n";
            earth();
            break;
    }
}
