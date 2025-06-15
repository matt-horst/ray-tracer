#pragma once
#include "hittable.hpp"
#include "vec3.hpp"
#include "color.hpp"
#include "texture.hpp"
#include <cmath>
#include <memory>

class Material {
public:
    virtual ~Material() = default;
    
    virtual bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const {
        return false;
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

private:
    std::shared_ptr<Texture> tex_;
};

class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        const auto reflected = normalize(reflect(ray_in.direction(), rec.normal)) + (fuzz * random_unit_vector());
        ray_out = Ray(rec.p, reflected, ray_in.time());
        attenuation = albedo;

        return (dot(ray_out.direction(), rec.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(double refraction_index) :refraction_index(refraction_index) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        attenuation = Color(1.0, 1.0, 1.0);
        const double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;
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

private:
    double refraction_index;

    // Schlick's approximation for reflectance
    static double reflectance(double cos_theta, double refraction_index) {
        auto r0 = (1.0 - refraction_index) / (1.0 + refraction_index);
        r0 = r0 * r0;

        return r0 + (1 - r0) * std::pow((1.0 - cos_theta), 5);
    }
};
