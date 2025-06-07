#pragma once
#include "hittable.hpp"
#include "vec3.hpp"
#include "color.hpp"

class Material {
public:
    virtual ~Material() = default;
    
    virtual bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const {
        return false;
    }
};

class Lambertian : public Material {
public:
    Lambertian(const Color& albedo) : albedo(albedo) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        // Catch degenerate scatter_direction
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }
        ray_out = Ray(rec.p, scatter_direction);
        attenuation = albedo;

        return true;
    }

private:
    Color albedo;
};

class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray<double>& ray_in, const HitRecord& rec, Color &attenuation, Ray<double>& ray_out) const override {
        const auto reflected = normalize(reflect(ray_in.direction(), rec.normal)) + (fuzz * random_unit_vector());
        ray_out = Ray(rec.p, reflected);
        attenuation = albedo;

        return (dot(ray_out.direction(), rec.normal) > 0);
    }

private:
    Color albedo;
    double fuzz;
};
