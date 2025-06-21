#pragma once

#include "thread_pool.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "hittable_list.hpp"
#include "camera.hpp"
#include <cstdint>
#include <optional>
#include <cassert>

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
    int32_t chunk_width_ = 0, chunk_height_ = 0;

private:
    friend struct YAML::convert<RenderSettings>;
};

void render(Image &img, const Camera& cam, const HittableList& scene, const RenderSettings &rs);

class RenderTaskGenerator : public TaskGenerator {
public:
    RenderTaskGenerator(Image& img, const Camera& cam, const HittableList& scene, const RenderSettings &rs) :
     img_(img), cam_(cam), scene_(scene), rs_(rs), num_chunks_(img.width_ * img.height_ / (rs.chunk_width_ * rs.chunk_height_)), chunks_per_row_(img.width_ / rs.chunk_width_) {
         assert(img.width_ % rs_.chunk_width_ == 0 && "Chunk width must be a factor of the image width.");
         assert(img.height_ % rs_.chunk_height_ == 0 && "Chunk height must be a factor of the image height.");
     }

    std::optional<std::function<void()>> next() override;

    bool has_next() const override { return current_chunk_ < num_chunks_; }

    double progress() const override { return static_cast<double>(current_chunk_) / num_chunks_; }

private:
    Image& img_;
    const Camera& cam_;
    const HittableList& scene_;
    const RenderSettings& rs_;
    int32_t current_chunk_ = 0;
    int32_t num_chunks_;
    int32_t chunks_per_row_;
};
