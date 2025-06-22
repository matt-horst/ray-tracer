#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <unistd.h>
#include <cassert>
#include <fstream>
#include "camera.hpp"
#include "hittable_list.hpp"
#include "render.hpp"
#include "image.hpp"
#include "scene.hpp"
#include "serialization.hpp"
#include "argparse/argparse.hpp"


int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("ray-tracer");

    program.add_argument("resolution")
        .help("resolution specified as `<width>x<height>` or `<width>@<aspect_ratio_w>:<aspect_ratio_h>`.");

    program.add_argument("scene")
        .help("loads YAML file specifiying scene.");

    program.add_argument("-r", "--render-settings")
        .help("loads YAML file specifiying rendering settings.");

    program.add_argument("-c", "--camera-settings")
        .help("loads YAML file specifiying camera settings.");

    program.add_argument("-n", "--num-threads")
        .help("number of threads of execution.")
        .scan<'i', uint32_t>();

    program.add_argument("-d", "--max-depth")
        .help("maximum depth of recursion.")
        .scan<'i', uint32_t>();

    program.add_argument("-s", "--samples-per-pixel")
        .help("number of rays cast at each pixel.")
        .scan<'i', uint32_t>();

    program.add_argument("-w", "--chunk-width")
        .help("width of the chunks when rendering with mutliple threads.")
        .scan<'i', int32_t>();

    program.add_argument("-h", "--chunk-height")
        .help("height of the chunks when rendering with mutliple threads.")
        .scan<'i', int32_t>();

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
    Image img;
    if (resolution.contains('@')) {
        const auto idx = resolution.find('@');
        const auto width = std::stoi(resolution.substr(0, idx));
        const auto aspect_ratio = resolution.substr(idx + 1);
        const auto idx2 = aspect_ratio.find(':');
        const auto ar_width = std::stoi(aspect_ratio.substr(0, idx2));
        const auto ar_height = std::stoi(aspect_ratio.substr(idx2 + 1));

        img = Image(width, AspectRatio(ar_width, ar_height));
    } else if (resolution.contains('x')) {
        const auto idx = resolution.find('x');
        const auto width = std::stoi(resolution.substr(0, idx));
        const auto height = std::stoi(resolution.substr(idx + 1));

        img = Image(width, height);
    } else {
        std::cerr << std::format("Unable to parse resolution `{}`.", resolution);;
        std::cerr << program.usage();
        return EXIT_FAILURE;
    }

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

    if (auto chunk_width = program.present<int32_t>("chunk-width")) {
        rs.chunk_width_ = *chunk_width;
    } else if (rs.chunk_width_ == 0) {
        rs.chunk_width_ = img.width_ / img.ar_.w_;

    }

    if (auto chunk_height = program.present<int32_t>("chunk-height")) {
        rs.chunk_height_ = *chunk_height;
    } else if (rs.chunk_height_ == 0) {
        rs.chunk_height_ = img.height_ / img.ar_.h_;
    }
    
    int divisor = 2;
    int max_divisor = std::sqrt(std::min(rs.chunk_width_, rs.chunk_height_));
    while (int32_t(rs.num_threads * 16) > img.width_ * img.height_ / (rs.chunk_width_ * rs.chunk_height_) && divisor < max_divisor) {
        if (rs.chunk_width_ % divisor == 0 && rs.chunk_height_ % divisor == 0) {
            rs.chunk_width_ /= divisor;
            rs.chunk_height_ /= divisor;
        } else {
            divisor++;
        }
    }

    Scene scene = LoadScene(program.get("scene"));

    if (auto file_name = program.present("camera-settings")) {
        scene.cb_ = LoadCamera(*file_name);
    }

    render(img, scene.camera(img), HittableList(scene.bvh()), rs);

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
