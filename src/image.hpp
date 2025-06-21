#pragma once

#include <cassert>
#include <iostream>
#include "color.hpp"

class ImageChunk {
public:
    int32_t x, y, width, height;
    std::vector<std::vector<Color>> &pixels;

    ImageChunk(int32_t x, int32_t y, int32_t w, int32_t h, std::vector<std::vector<Color>> &pixels) : x(x), y(y), width(w), height(h), pixels(pixels) {}
};

class Image {
public:
    int32_t width, height;
    int32_t num_chunks;

    ImageChunk get(size_t id) {
        return ImageChunk(id / chunks_per_row * chunk_height, (id % chunks_per_row) * chunk_width, chunk_width, chunk_height, pixels);
    }

    std::ostream& write(std::ostream& out) const {
        out << "P3\n";
        out << width << ' ' << height << '\n';
        out << "255\n";

        for (int32_t i = 0; i < height; i++) {
            for (int32_t j = 0; j < width; j++) {
                const Color &color = pixels[i][j];
                out << color << '\n';
            }
        }

        return out;
    }

    Image(int32_t width, int32_t height, int32_t chunk_width, int32_t chunk_height) :
        width(width),
        height(height),
        num_chunks(width * height  / (chunk_width * chunk_height)),
        chunk_width(chunk_width),
        chunk_height(chunk_height),
        chunks_per_row(width / chunk_width),
        pixels(std::vector<std::vector<Color>>(height, std::vector<Color>(width)))
    {
        assert(width % chunk_width == 0 && "Chunk width must be a factor of total width.");
        assert(height % chunk_height == 0 &&  "Chunk height must be a factor of total height.");
    }

private:
    int32_t chunk_width, chunk_height;
    int32_t chunks_per_row;
    std::vector<std::vector<Color>> pixels;
};


inline std::ostream& operator<<(std::ostream& out, const Image &img) {
    return img.write(out);
}
