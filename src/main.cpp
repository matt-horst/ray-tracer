#include <iostream>
#include <memory>
#include <unistd.h>
#include "camera.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
#include "material.hpp"

int main(void) {
    CameraParams params;
    params.image_width = 800;
    params.samples_per_pixel = 100;
    params.max_depth = 50;
    const Camera cam(params);

    const auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    const auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    const auto material_left = std::make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
    const auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    HittableList world;
    world.add(std::make_shared<Sphere>(Point3<double>(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<Sphere>(Point3<double>(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(std::make_shared<Sphere>(Point3<double>(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(std::make_shared<Sphere>(Point3<double>(1.0, 0.0, -1.0), 0.5, material_right));

    cam.render(world);
}
