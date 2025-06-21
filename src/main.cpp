#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include "camera.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "serialization.hpp"
#include "argparse/argparse.hpp"

void bouncing_spheres() {
    Image img(400, 225, 25, 25);

    CameraBuilder cb = LoadCamera("examples/camera1.yaml");

    const Camera cam = cb.build(img);

    const RenderSettings rs = LoadRenderSettings("examples/render_fast.yaml");

    YAML::Node node = YAML::LoadFile("examples/bouncing_spheres.yaml");
    Scene scene = node.as<Scene>();

    render(img, cam, HittableList(scene.bvh()), rs);

    std::cout << img;
}

void checkered_spheres() {
    Image img(400, 225, 25, 25);
    HittableList scene;

    const auto checker_tex = std::make_shared<CheckerTexture>(0.32, Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));

    scene.add(std::make_shared<Sphere>(Point3<double>(0, -10, 0), 10, std::make_shared<Lambertian>(checker_tex)));
    scene.add(std::make_shared<Sphere>(Point3<double>(0, 10, 0), 10, std::make_shared<Lambertian>(checker_tex)));

    RenderSettings rs(100, 50);

    CameraBuilder cb;
    cb.vfov_ = 20;
    cb.lookfrom_ = Point3<double>(13, 2, 3);
    cb.lookat_ = Point3<double>(0, 0, 0);
    cb.vup_ = Vec3<double>(0, 1, 0);
    cb.defocus_angle_ = 0;
    
    const Camera cam(cb.build(img));

    render(img, cam, scene, rs);

    std::cout << img;
}

void earth() {
    Image img(400, 225, 25, 25);

    const auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
    const auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    const auto globe = std::make_shared<Sphere>(Point3<double>(0, 0, 0), 2, earth_surface);
    
    const RenderSettings rs(100, 50);

    CameraBuilder cb;
    cb.vfov_ = 20;
    cb.lookfrom_ = Point3<double>(0, 0, 12);
    cb.lookat_ = Point3<double>(0, 0, 0);
    cb.vup_ = Vec3<double>(0, 1, 0);
    cb.defocus_angle_ = 0;
    
    const Camera cam(cb.build(img));

    render(img, cam, HittableList(globe), rs);

    std::cout << img;
}


int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("ray-tracer");

    program.add_argument("resolution")
        .help("resolution specified as `<width>x<height>` or `<width>@<aspect_ratio_w>:<aspect_ratio_h>`.");

    program.add_argument("scene")
        .help("loads YAML file specifiying scene.");

    program.add_argument("-r", "--render-settings")
        .help("loads YAML file specifiying rendering settings.");

    program.add_argument("-c", "--camera-settings")
        .help("loads YAML file specifiying camera settings.")
        .required();

    program.add_argument("-n", "--num-threads")
        .help("number of threads of execution.")
        .scan<'i', uint32_t>();

    program.add_argument("-d", "--max-depth")
        .help("maximum depth of recursion.")
        .scan<'i', uint32_t>();

    program.add_argument("-s", "--samples-per-pixel")
        .help("number of rays cast at each pixel.")
        .scan<'i', uint32_t>();

    program.add_argument("-o", "--output")
        .help("output file.");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return EXIT_FAILURE;
    }

    const auto resolution = program.get<std::string>("resolution");
    int32_t width, height;
    if (resolution.contains('@')) {
        const auto idx = resolution.find('@');
        width = std::stoi(resolution.substr(0, idx));
        const auto aspect_ratio = resolution.substr(idx + 1);
        const auto idx2 = aspect_ratio.find(':');
        const auto ar_width = std::stod(aspect_ratio.substr(0, idx2));
        const auto ar_height = std::stod(aspect_ratio.substr(idx2 + 1));

        const auto ar = ar_width / ar_height;
        height = std::max(static_cast<int32_t>(width / ar), 1);
    } else if (resolution.contains('x')) {
        const auto idx = resolution.find('x');
        width = std::stoi(resolution.substr(0, idx));
        height = std::stoi(resolution.substr(idx + 1));
    } else {
        std::cerr << std::format("Unable to parse resolution `{}`.", resolution);;
        std::cerr << program.usage();
        return EXIT_FAILURE;
    }

    Image img(width, height, 25, 25);

    RenderSettings rs;
    if (auto file_name = program.present("render-settings")) {
        rs = LoadRenderSettings(*file_name);
    }

    if (auto num_threads = program.present<uint32_t>("num-threads")) {
        rs.num_threads = *num_threads;
    }
    
    if (auto samples_per_pixel = program.present<uint32_t>("samples-per-pixel")) {
        rs.set_samples_per_pixel(*samples_per_pixel);
    }

    if (auto max_depth = program.present<uint32_t>("max-depth")) {
        rs.max_depth_ = *max_depth;
    }

    CameraBuilder cb;
    if (auto file_name = program.present("camera-settings")) {
        cb = LoadCamera(*file_name);
    }

    Camera cam = cb.build(img);

    Scene scene = LoadScene(program.get("scene"));

    render(img, cam, HittableList(scene.bvh()), rs);

    if (auto file_name = program.present("output")) {
        std::ofstream file(*file_name);
        if (file.is_open()) {
            std::clog << "Writing to file: " << *file_name << "...\n";
            file << img;
            std::clog << "Successfully written to file!\n";
        } else {
            std::cerr << "Failed to open file: " << *file_name << ".\n";
            return EXIT_FAILURE;
        }
        file.close();
    } else {
        std::cout << img;
    }

    return EXIT_SUCCESS;
}
