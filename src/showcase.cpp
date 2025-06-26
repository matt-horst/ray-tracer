#include "bvh.hpp"
#include "constant_medium.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "point3.hpp"
#include "render.hpp"
#include "serialization.hpp"
#include "sphere.hpp"
#include "texture.hpp"
#include "util.hpp"
#include <memory>

int main() {
    Scene scene;

    const auto ground = std::make_shared<Lambertian>(Color(0.48, 0.83, 0.53));
    scene.add_material(ground);

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            const auto w = 100.0;
            const auto x0 = -1000.0 + i * w;
            const auto z0 = -1000.0 + j * w;
            const auto y0 = 0.0;

            const auto x1 = x0 + w;
            const auto y1 = random_double(1, 101);
            const auto z1 = z0 + w;

            scene.add_object(std::make_shared<Box>(Point3<double>(x0, y0, z0), Point3<double>(x1, y1, z1), ground));
        }
    }

    const auto light = std::make_shared<DiffuseLight>(Color(7.0, 7.0, 7.0));
    scene.add_object(std::make_shared<Quad>(Point3<double>(123, 554, 147), Vec3<double>(300, 0, 0), Vec3<double>(0, 0, 265), light));

    const auto center_1 = Point3<double>(400, 400, 200);
    const auto center_2 = center_1 + Vec3<double>(30, 0, 0);
    const auto sphere_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    scene.add_object(std::make_shared<Sphere>(center_1, center_2, 50, sphere_material));

    const auto glass = std::make_shared<Dielectric>(1.5);
    scene.add_material(glass);

    scene.add_object(std::make_shared<Sphere>(Point3<double>(260, 150, 45), 50, glass));

    scene.add_object(std::make_shared<Sphere>(Point3<double>(0, 150, 145), 50, std::make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)));

    const auto boundary_1 = std::make_shared<Sphere>(Point3<double>(360, 150, 145), 70, glass);
    scene.add_object(boundary_1);
    scene.add_object(std::make_shared<ConstantMedium>(boundary_1, 0.2, Color(0.2, 0.4, 0.9)));

    const auto boundary_2 = std::make_shared<Sphere>(Point3<double>(), 5000, glass);
    scene.add_object(boundary_2);
    scene.add_object(std::make_shared<ConstantMedium>(boundary_2, 0.0001, Color(1.0, 1.0, 1.0)));

    const auto globe_mat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("images/earthmap.jpg"));
    scene.add_object(std::make_shared<Sphere>(Point3<double>(400, 200, 400), 100, globe_mat));

    const auto perlin_tex = std::make_shared<NoiseTexture>(0.2);
    scene.add_object(std::make_shared<Sphere>(Point3<double>(220, 280, 300), 80, std::make_shared<Lambertian>(perlin_tex)));

    const auto white = std::make_shared<Lambertian>(Color(0.73, 0.73, 0.73));
    scene.add_material(white);

    int ns = 1000;
    HittableList boxes;
    for (int i = 0; i < ns; i++) {
        boxes.add(std::make_shared<Sphere>(Vec3<double>::random(0, 165), 10, white));
    }

    scene.add_object(std::make_shared<Translate>(std::make_shared<RotateY>(std::make_shared<BVHNode>(boxes), 15), Vec3<double>(-100, 270, 395)));

    scene.cb_.vfov_ = 40;
    scene.cb_.lookfrom_ = Point3<double>(478, 278, -600);
    scene.cb_.lookat_ = Point3<double>(278, 278, 0);
    scene.cb_.vup_ = Vec3<double>(0, 1, 0);
    scene.cb_.defocus_angle_ = 0;

    std::cout << scene;
}
