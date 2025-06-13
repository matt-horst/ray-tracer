#pragma once

#include "thread_pool.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"
#include <optional>

Color ray_color(const Ray<double> &ray, const Hittable &world, int32_t depth, int32_t max_depth);
void render_chunk(const Camera& cam, const HittableList& scene, ImageChunk img);
void render(const Camera& cam, const HittableList& scene, int num_threads);

class RenderTaskGenerator : public TaskGenerator {
public:
    RenderTaskGenerator(Image& img, const Camera& cam, const HittableList& scene) :
     img(img), cam(cam), scene(scene) { }

    std::optional<std::function<void()>> next() override {
        if (current_chunk < img.num_chunks) {
            ImageChunk chunk = img.get(current_chunk++);
            return [this, chunk = std::move(chunk)] { render_chunk(cam, scene, chunk); };
        }
        return std::nullopt;
    }

    bool has_next() override {
        return false;
    }

private:
    Image& img;
    const Camera& cam;
    const HittableList& scene;
    int32_t current_chunk = 0;
};
