#include "serialization.hpp"
#include <iostream>
// void checkered_spheres() {
//     Image img(400, 225, 25, 25);
//     HittableList scene;
//
//     const auto checker_tex = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
//
//     scene.add(std::make_shared<Sphere>(Point3<double>(0, -10, 0), 10, std::make_shared<Lambertian>(checker_tex)));
//     scene.add(std::make_shared<Sphere>(Point3<double>(0, 10, 0), 10, std::make_shared<Lambertian>(checker_tex)));
//
//     RenderSettings rs(100, 50);
//
//     CameraBuilder cb;
//     cb.vfov_ = 20;
//     cb.lookfrom_ = Point3<double>(13, 2, 3);
//     cb.lookat_ = Point3<double>(0, 0, 0);
//     cb.vup_ = Vec3<double>(0, 1, 0);
//     cb.defocus_angle_ = 0;
//
//     const Camera cam(cb.build(img));
//
//     render(img, cam, scene, rs);
//
//     std::cout << img;
// }
//
int main() {
    Scene scene;

    const auto checker_tex = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    scene.add_texture(checker_tex);

    const auto mat = std::make_shared<Lambertian>(checker_tex);
    scene.add_material(mat);

    
    scene.add_object(std::make_shared<Sphere>(Point3<double>(0, -10, 0), 10, mat));
    scene.add_object(std::make_shared<Sphere>(Point3<double>(0, 10, 0), 10, mat));

    std::cout << scene << '\n';
}
