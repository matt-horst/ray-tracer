#pragma once

#include "thread_pool.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"
#include <optional>

class RenderSettings {
public:
    RenderSettings(int32_t samples_per_pixel, int32_t max_depth) : samples_per_pixel_(samples_per_pixel), pixel_color_scale_(1.0 / samples_per_pixel_), max_depth_(max_depth) {}
    RenderSettings() {}

    void set_samples_per_pixel(uint32_t samples_per_pixel) {
        samples_per_pixel_ = samples_per_pixel;
        pixel_color_scale_ = 1.0 / samples_per_pixel_;
    }

    int32_t samples_per_pixel_ = 100;
    double pixel_color_scale_ = 1.0 / 100.0;
    int32_t max_depth_ = 50;
    uint32_t num_threads = std::thread::hardware_concurrency();

private:
    friend struct YAML::convert<RenderSettings>;
};

void render(Image &img, const Camera& cam, const HittableList& scene, const RenderSettings &rs);

class RenderTaskGenerator : public TaskGenerator {
public:
    RenderTaskGenerator(Image& img, const Camera& cam, const HittableList& scene, const RenderSettings &rs) :
     img_(img), cam_(cam), scene_(scene), rs_(rs) { }

    std::optional<std::function<void()>> next() override;

    bool has_next() override {
        return current_chunk_ < img_.num_chunks;
    }

private:
    Image& img_;
    const Camera& cam_;
    const HittableList& scene_;
    const RenderSettings& rs_;
    int32_t current_chunk_ = 0;
};
