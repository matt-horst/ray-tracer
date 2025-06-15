#include "render.hpp"
#include "material.hpp"

Color ray_color(const Ray<double> &ray, const Hittable &world, int32_t depth, int32_t max_depth);
void render_chunk(const Camera& cam, const HittableList& scene, ImageChunk img);

Color ray_color(const Ray<double> &ray, const Hittable &world, int32_t depth, int32_t max_depth) {
    if (depth >= max_depth) return Color();

    HitRecord rec;
    if (world.hit(ray, Interval(0.001, infinity), rec)) {
        Ray<double> scattered;
        Color attenuation;
        if (rec.mat->scatter(ray, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth + 1, max_depth);
        }

        return Color();
    }

    const Vec3<double> unit_direction = normalize(ray.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void render_chunk(const Camera& cam, const HittableList& scene, ImageChunk img) {
    for (int32_t i = img.x; i < img.x + img.height; i++) {
        for (int32_t j = img.y; j < img.y + img.width; j++) {
            Color pixel_color(0.0, 0.0, 0.0);
            for (int32_t k = 0; k < cam.samples_per_pixel; k++) {
                Ray<double> ray = cam.cast_ray_at_pixel_loc(i, j);
                pixel_color += ray_color(ray, scene, 0, cam.max_depth);
            }

            img.pixels[i][j] = pixel_color * cam.pixel_color_scale;
        }
    }
}

void render(const Camera& cam, const HittableList& scene, int num_threads) {
    Image img(cam.image_width, cam.image_height, cam.image_width / 16, cam.image_height / 9);
    RenderTaskGenerator gen(img, cam, scene);
    ThreadPool pool(gen, num_threads);

    // Wait
    int32_t count = 0;
    std::clog << std::format("Running on {} threads...\n", pool.num_threads);
    while ((count = pool.count()) < img.num_chunks) {
        const float percentage = (100.0 * count) / img.num_chunks;
        std::clog << std::format("\r{:.2f}% ", percentage) << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::clog << std::format("\r{:.2f}% \n", 100.0);

    // Write image to file
    img.write();
}

std::optional<std::function<void()>> RenderTaskGenerator::next() {
    if (has_next()) {
        ImageChunk chunk = img.get(current_chunk++);
        return [this, chunk = std::move(chunk)] { render_chunk(cam, scene, chunk); };
    }
    return std::nullopt;
}
