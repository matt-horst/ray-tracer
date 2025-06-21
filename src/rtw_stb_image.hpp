#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"
#pragma GCC diagnostic pop

#include <cstdlib>
#include <iostream>
#include <string>

class RTWImage {
public:
    RTWImage() {}
    RTWImage(const char *image_file_name) {
        const auto file_name = std::string(image_file_name);
        const auto image_dir = getenv("RTW_IMAGES");

        if (image_dir && load(std::string(image_dir) + "/" + image_file_name)) return;
        if (load(file_name)) return;
        if (load("images/" + file_name)) return;
        if (load("../images/" + file_name)) return;
        if (load("../../images/" + file_name)) return;
        if (load("../../../images/" + file_name)) return;
        if (load("../../../../images/" + file_name)) return;

        std::cerr << "ERROR: Could not load image file'" << image_file_name << "'.\n";
    }
    
    ~RTWImage() {
        delete[] bdata;
        stbi_image_free(fdata);
    }

    bool load(const std::string& file_name) {
        auto n = bytes_per_pixel;
        fdata = stbi_loadf(file_name.c_str(), &image_width, &image_height, &n, bytes_per_pixel);

        if (fdata == nullptr) return false;

        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }

    int width() const { return (fdata != nullptr) ? image_width : 0; }
    int height() const { return (fdata != nullptr) ? image_height : 0; }

    const unsigned char *pixel_data(int x, int y) const {
        static unsigned char magenta[] = { 255, 0, 255 };
        if (bdata == nullptr) return magenta;

        x = clamp(x, 0, image_width);
        y = clamp(y, 0, image_height);

        return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
    }
private:
    const int bytes_per_pixel = 3;
    float *fdata = nullptr;
    unsigned char *bdata = nullptr;
    int image_width = 0;
    int image_height = 0;
    int bytes_per_scanline = 0;

    void convert_to_bytes() {
        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];

        auto *bptr = bdata;
        auto *fptr = fdata;

        for (auto i = 0; i < total_bytes; i++) {
            *bptr++ = float_to_byte(*fptr++);
        }
    }

    static int clamp(int x, int low, int high) {
        if (x < low) return low;
        if (x >= high) return high - 1;
        return x;
    }

    static unsigned char float_to_byte(float f) {
        if (f <= 0.0) {
            return 0;
        }

        if (f >= 1.0) {
            return 255;
        }

        return static_cast<unsigned char>(256.0 * f);
    }
};
