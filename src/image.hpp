#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include "color.hpp"

class AspectRatio {
public:
    int32_t w_, h_;

    AspectRatio(int32_t w, int32_t h) {
        int32_t max_divisor = static_cast<int32_t>(std::sqrt(static_cast<double>(std::min(w, h))));
        for (int32_t d = 2; d < max_divisor; d++) {
            while (w % d == 0 && h % d == 0) {
                w /= d;
                h /= d;
            }
        }

        w_ = w;
        h_ = h;
    }
};

class ImageChunk {
public:
    int32_t x, y, width, height;
    std::vector<std::vector<Color>> &pixels;

    ImageChunk(int32_t x, int32_t y, int32_t w, int32_t h, std::vector<std::vector<Color>> &pixels) : x(x), y(y), width(w), height(h), pixels(pixels) {}
};

class Image {
public:
    int32_t width_, height_;
    AspectRatio ar_;
    std::vector<std::vector<Color>> pixels_;

    Image(int32_t width, int32_t height) : width_(width), height_(height), ar_(width_, height_), pixels_(std::vector<std::vector<Color>>(height_, std::vector<Color>(width_))) {}
    Image(int32_t width, AspectRatio ar) : Image(width, width / ar.w_ * ar.h_) {}
    Image() : Image(400, 225) {}


    std::ostream& write(std::ostream& out) const {
        out << "P3\n";
        out << width_ << ' ' << height_ << '\n';
        out << "255\n";

        for (int32_t i = 0; i < height_; i++) {
            for (int32_t j = 0; j < width_; j++) {
                const Color &color = pixels_[i][j];
                out << color << '\n';
            }
        }

        return out;
    }
};


inline std::ostream& operator<<(std::ostream& out, const Image &img) {
    return img.write(out);
}
