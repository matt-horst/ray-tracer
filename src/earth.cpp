#include "serialization.hpp"


int main() {
    const auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
    const auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    const auto globe = std::make_shared<Sphere>(Point3<double>(0, 0, 0), 2, earth_surface);

    Scene scene;
    scene.add_texture(earth_texture);
    scene.add_material(earth_surface);
    scene.add_object(globe);

    std::cout << scene << '\n';
}
