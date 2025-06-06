#include <iostream>
#include <memory>
#include <unistd.h>
#include "camera.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"

int main(void) {
    CameraParams params;
    params.image_width = 800;
    params.samples_per_pixel = 100;
    params.max_depth = 100;
    const Camera cam(params);

    HittableList world;
    world.add(std::make_shared<Sphere>(Point3<double>(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(Point3<double>(0, -100.5, -1), 100));

    cam.render(world);
}
