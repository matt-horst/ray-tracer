#pragma once
#include "color.hpp"
#include "vec3.hpp"
#include "point3.hpp"
#include "rtw_stb_image.hpp"
#include "yaml-cpp/yaml.h"
#include "perlin.hpp"
#include <memory>

class Texture {
public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Point3<double>& p) const = 0;
};

class SolidColorTexture : public Texture {
public:
    SolidColorTexture(const Color &color) : color_(color) {}
    SolidColorTexture(double r, double g, double b) : color_(r, g, b) {}

    Color value(double u, double v, const Point3<double>& p) const override { return color_; }

private:
    Color color_;
    friend struct YAML::convert<std::shared_ptr<SolidColorTexture>>;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd) : 
        scale_(scale), inv_scale_(1.0 / scale), even_(even), odd_(odd) {}
    CheckerTexture(double scale, Color even, Color odd) : 
        CheckerTexture(scale, std::make_shared<SolidColorTexture>(even), std::make_shared<SolidColorTexture>(odd)) {}

    Color value(double u, double v, const Point3<double>& p) const override {
        const auto x_int = int(std::floor(inv_scale_ * p.x()));
        const auto y_int = int(std::floor(inv_scale_ * p.y()));
        const auto z_int  = int(std::floor(inv_scale_ * p.z()));

        bool is_even = (x_int + y_int + z_int) % 2 == 0;

        return is_even ? even_->value(u, v, p) : odd_->value(u, v, p);
    }

private:
    double scale_;
    double inv_scale_;
    std::shared_ptr<Texture> even_;
    std::shared_ptr<Texture> odd_;
    friend struct YAML::convert<std::shared_ptr<CheckerTexture>>;
};

class ImageTexture : public Texture {
public:
    ImageTexture(const char *file_name) : file_name_(file_name), image_(file_name) {}

    Color value(double u, double v, const Point3<double>& p) const override {
        if (image_.height() <= 0) return Color(0, 1, 1);

        u = Interval(0, 1).clamp(u);
        v = 1.0 - Interval(0, 1).clamp(v);

        const auto i = int(u * image_.width());
        const auto j = int(v * image_.height());
        const auto pixel = image_.pixel_data(i, j);

        const auto color_scale = 1.0 / 255.0;
        return Color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
    }

private:
    std::string file_name_;
    RTWImage image_;
    friend struct YAML::convert<std::shared_ptr<ImageTexture>>;
};

class NoiseTexture : public Texture {
public:
    NoiseTexture(double scale) : scale_(scale) {}

    Color value(double u, double v, const Point3<double> &p) const override {
        // return Color(1.0, 1.0, 1.0) * 0.5 * (1.0 + noise_.noise(scale_ * p));
        // return Color(1.0, 1.0, 1.0) * noise_.turb(p, 7);
        return Color(0.5, 0.5, 0.5) * (1 + std::sin(scale_ * p.z() + 10 * noise_.turb(p, 7)));
    }

private:
    Perlin noise_;
    double scale_;

    friend struct YAML::convert<std::shared_ptr<NoiseTexture>>;
};
