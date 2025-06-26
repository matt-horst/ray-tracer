#include "render.hpp"
#include "image.hpp"
#include "material.hpp"

Color ray_color(const Ray<double> &ray, const Hittable &world, const Color &background, int32_t depth, int32_t max_depth);
void render_chunk(const Camera& cam, const HittableList& scene, ImageChunk img);

Color ray_color(const Ray<double> &ray, const Hittable &world, const Color &background, int32_t depth, int32_t max_depth) {
    if (depth >= max_depth) return Color();

    HitRecord rec;
    if (!world.hit(ray, Interval(0.001, infinity), rec)) {
        return background;
    }

    const Color color_emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

    Ray<double> scattered;
    Color attenuation;
    if (!rec.mat->scatter(ray, rec, attenuation, scattered)) {
        return color_emitted;
    }

    const Color color_scattered = attenuation * ray_color(scattered, world, background, depth + 1, max_depth);

    return color_emitted + color_scattered;
}

void render_chunk(const Camera& cam, const HittableList& scene, const RenderSettings &rs, ImageChunk img) {
    for (int32_t i = img.x; i < img.x + img.height; i++) {
        for (int32_t j = img.y; j < img.y + img.width; j++) {
            Color pixel_color(0.0, 0.0, 0.0);
            for (int32_t k = 0; k < rs.samples_per_pixel_; k++) {
                Ray<double> ray = cam.cast_ray_at_pixel_loc(i, j);
                pixel_color += ray_color(ray, scene, cam.background_, 0, rs.max_depth_);
            }

            img.pixels[i][j] = pixel_color * rs.pixel_color_scale_;
        }
    }
}

void render(Image &img, const Camera& cam, const HittableList& scene, const RenderSettings &rs) {
    RenderTaskGenerator gen(img, cam, scene, rs);
    ThreadPool pool(gen, rs.num_threads);

    // Wait 
    std::clog << std::format("Running on {} threads...\n", pool.num_threads);
    while (pool.has_next()) {
        const float percentage = 100.0 * pool.progress();
        std::clog << std::format("\r{:.2f}% ", percentage) << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::clog << std::format("\r{:.2f}% \n", 100.0);
}

std::optional<std::function<void()>> RenderTaskGenerator::next() {
    if (has_next()) {
        ImageChunk chunk = ImageChunk(current_chunk_ / chunks_per_row_ * rs_.chunk_height_, (current_chunk_ % chunks_per_row_) * rs_.chunk_width_, rs_.chunk_width_, rs_.chunk_height_, img_.pixels_);
        current_chunk_++;
        return [this, chunk = std::move(chunk)] { render_chunk(cam_, scene_, rs_, chunk); };
    }
    return std::nullopt;
}
