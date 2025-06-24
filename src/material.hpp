#pragma once
#include "hittable.hpp"
#include "vec3.hpp"
#include "color.hpp"
#include "texture.hpp"
#include "yaml-cpp/node/convert.h"
#include <cmath>
#include <memory>

class Material {
public:
    virtual ~Material() = default;
    
    virtual bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const {
        return false;
    }

    virtual Color emitted(double u, double v, const Point3<double> &p) const {
        return Color();
    }
};

class Lambertian : public Material {
public:
    Lambertian(const Color& albedo) : tex_(std::make_shared<SolidColorTexture>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> tex) : tex_(tex) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        // Catch degenerate scatter_direction
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }
        ray_out = Ray(rec.p, scatter_direction, ray_in.time());
        attenuation = tex_->value(rec.u, rec.v, rec.p);

        return true;
    }

    inline static const std::string NAME = "lambertian";

private:
    std::shared_ptr<Texture> tex_;
    friend struct YAML::convert<std::shared_ptr<Lambertian>>;
};

class Metal : public Material {
public:
    Metal(const Color& color, double fuzz) : color_(color), fuzz_(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        const auto reflected = normalize(reflect(ray_in.direction(), rec.normal)) + (fuzz_ * random_unit_vector());
        ray_out = Ray(rec.p, reflected, ray_in.time());
        attenuation = color_;

        return (dot(ray_out.direction(), rec.normal) > 0);
    }

    inline static const std::string NAME = "metal";

private:
    Color color_;
    double fuzz_;
    friend struct YAML::convert<std::shared_ptr<Metal>>;
};

class Dielectric : public Material {
public:
    Dielectric(double refraction_index) : refraction_index_(refraction_index) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        const double ri = rec.front_face ? (1.0 / refraction_index_) : refraction_index_;
        const auto unit_dir = normalize(ray_in.direction());
        const auto cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
        const auto sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
        const bool can_refract = ri * sin_theta <= 1.0;

        Vec3<double> direction;
        if (can_refract && reflectance(cos_theta, ri) <= random_double()) {
            // Refraction
            direction = refract(unit_dir, rec.normal, ri);
        } else {
            // Reflection
            direction = reflect(unit_dir, rec.normal);
        }

        ray_out = Ray(rec.p, direction, ray_in.time());
        return true;
    }
    
    inline static const std::string NAME = "dielectic";

private:
    double refraction_index_;

    // Schlick's approximation for reflectance
    static double reflectance(double cos_theta, double refraction_index) {
        auto r0 = (1.0 - refraction_index) / (1.0 + refraction_index);
        r0 = r0 * r0;

        return r0 + (1 - r0) * std::pow((1.0 - cos_theta), 5);
    }

    friend struct YAML::convert<std::shared_ptr<Dielectric>>;
};

class DiffuseLight : public Material {
public:
    DiffuseLight(std::shared_ptr<Texture> tex) : tex_(tex) {}
    DiffuseLight(const Color &color) : tex_(std::make_shared<SolidColorTexture>(color)) {}

    Color emitted(double u, double v, const Point3<double> &p) const override {
        return tex_->value(u, v, p);
    }

    inline static const std::string NAME = "diffuse-light";
private:
    std::shared_ptr<Texture> tex_;

    friend struct YAML::convert<std::shared_ptr<DiffuseLight>>;
};

class Isotropic : public Material {
public:
    Isotropic(std::shared_ptr<Texture> tex) : tex_(tex) {}
    Isotropic(const Color &color) : tex_(std::make_shared<SolidColorTexture>(color)) {}

    bool scatter(const Ray<double> &ray_in, const HitRecord &rec, Color &attenuation, Ray<double> &ray_out) const override {
        ray_out = Ray<double>(rec.p, random_unit_vector(), ray_in.time());
        attenuation = tex_->value(rec.u, rec.v, rec.p);
        return true;
    }

    inline static const std::string NAME = "isotropic";

private:
    std::shared_ptr<Texture> tex_;

    friend struct YAML::convert<std::shared_ptr<Isotropic>>;
};
