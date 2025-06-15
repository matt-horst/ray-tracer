#pragma once

#include "thread_pool.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"
#include <optional>

void render(const Camera& cam, const HittableList& scene, int num_threads);

class RenderTaskGenerator : public TaskGenerator {
public:
    RenderTaskGenerator(Image& img, const Camera& cam, const HittableList& scene) :
     img(img), cam(cam), scene(scene) { }

    std::optional<std::function<void()>> next() override;

    bool has_next() override {
        return current_chunk < img.num_chunks;
    }

private:
    Image& img;
    const Camera& cam;
    const HittableList& scene;
    int32_t current_chunk = 0;
};
