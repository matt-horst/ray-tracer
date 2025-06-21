#include "material.hpp"
#include "scene.hpp"
#include "serialization.hpp"
#include "texture.hpp"
#include <iostream>
#include <memory>

int main() {
    Scene scene;
    const auto solid_1 = std::make_shared<SolidColorTexture>(0.9, 0.9, 0.9);
    scene.add_texture(solid_1);

    const auto solid_2 = std::make_shared<SolidColorTexture>(0.2, 0.3, 0.1);
    scene.add_texture(solid_2);

    const auto checker_tex = std::make_shared<CheckerTexture>(0.32, solid_1, solid_2);
    scene.add_texture(checker_tex);

    const auto ground_mat = std::make_shared<Lambertian>(checker_tex);
    scene.add_material(ground_mat);
    scene.add_object(std::make_shared<Sphere>(Point3<double>(0.0, -1000.0, 0.0), 1000, ground_mat));

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
                    scene.add_object(std::make_shared<Sphere>(center, center_2.value(), 0.2, sphere_material));
                } else {
                    scene.add_object(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    scene.add_object(std::make_shared<Sphere>(Point3<double>(0, 1, 0), 1.0, std::make_shared<Dielectric>(1.5)));

    scene.add_object(std::make_shared<Sphere>(Point3<double>(-4, 1, 0), 1.0, std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1))));

    const auto metal_2 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    scene.add_object(std::make_shared<Sphere>(Point3<double>(4, 1, 0), 1.0, metal_2));

    std::cout << scene << '\n';
}
