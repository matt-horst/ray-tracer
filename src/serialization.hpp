#pragma once

#include "bvh.hpp"
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "point3.hpp"
#include "render.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "quad.hpp"
#include "constant_medium.hpp"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/yaml.h"
#include <cstdint>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <unordered_map>


namespace YAML {
template<typename T>
struct convert<Vec3<T>> {
    static Node encode(const Vec3<T> &rhs) {
        Node node;
        node.push_back(rhs.x());
        node.push_back(rhs.y());
        node.push_back(rhs.z());

        return node;
    }

    static bool decode(const Node &node, Vec3<T> &rhs) {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs[0] = node[0].as<T>();
        rhs[1] = node[1].as<T>();
        rhs[2] = node[2].as<T>();

        return true;
    }
};

template<>
struct convert<Color> {
    static Node encode(const Color &rhs) {
        Node node;
        node.push_back(rhs.r());
        node.push_back(rhs.g());
        node.push_back(rhs.b());
        return node;
    }

    static bool decode(const Node &node, Color &rhs) {
        if (!node.IsSequence() || node.size() != 3) { return false; }

        rhs.elem[0] = node[0].as<double>();
        rhs.elem[1] = node[1].as<double>();
        rhs.elem[2] = node[2].as<double>();

        return true;
    }
};

template<>
struct convert<std::shared_ptr<SolidColorTexture>> {
    static Node encode(const std::shared_ptr<SolidColorTexture> &rhs) {
        Node node;
        node["type"] = "solid_color";
        node["color"] = rhs->color_;
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<SolidColorTexture> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "solid_color") return false;

        rhs = std::make_shared<SolidColorTexture>(node["color"].as<Color>());
        return true;
    }
};

template<>
struct convert<std::shared_ptr<CheckerTexture>> {
    static Node encode(const std::shared_ptr<CheckerTexture> &rhs) {
        Node node;
        node["type"] = "checker";
        node["scale"] = rhs->scale_;
        node["even"] = rhs->even_;
        node["odd"] = rhs->odd_;
        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> &refs, const std::shared_ptr<CheckerTexture> &rhs) {
        Node node;
        node["type"] = "checker";
        node["scale"] = rhs->scale_;
        try {
            node["even"] = refs.at(rhs->even_);
        } catch (std::out_of_range &_) {
            node["even"] = rhs->even_;
        }

        try {
            node["odd"] = refs.at(rhs->odd_);
        } catch (std::out_of_range &_) {
            node["odd"] = rhs->odd_;
        }
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<CheckerTexture> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "checker") return false;

        const auto scale = node["scale"].as<double>();
        const auto even = node["even"].as<std::shared_ptr<Texture>>();
        const auto odd = node["odd"].as<std::shared_ptr<Texture>>();

        rhs = std::make_shared<CheckerTexture>(scale, even, odd);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &refs, std::shared_ptr<CheckerTexture> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "checker") return false;

        const auto scale = node["scale"].as<double>();

        std::shared_ptr<Texture> even;
        if (node["even"].IsScalar()) {
            even = refs.at(node["even"].as<std::string>());
        } else {
            even = node["even"].as<std::shared_ptr<Texture>>();
        }

        std::shared_ptr<Texture> odd;
        if (node["odd"].IsScalar()) {
            odd = refs.at(node["odd"].as<std::string>());
        } else {
            odd = node["odd"].as<std::shared_ptr<Texture>>();
        }

        rhs = std::make_shared<CheckerTexture>(scale, even, odd);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<ImageTexture>> {
    static Node encode(const std::shared_ptr<ImageTexture> &rhs) {
        Node node;
        node["type"] = "image";
        node["file_name"] = rhs->file_name_;
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<ImageTexture> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "image") return false;

        rhs = std::make_shared<ImageTexture>(node["file_name"].as<std::string>().c_str());

        return true;
    }
};

template<>
struct convert<std::shared_ptr<NoiseTexture>> {
    static Node encode(const std::shared_ptr<NoiseTexture> &rhs) {
        Node node;

        node["type"] = "noise";
        node["scale"] = rhs->scale_;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<NoiseTexture> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "noise") return false;

        const auto scale = node["scale"].as<double>();
        rhs = std::make_shared<NoiseTexture>(scale);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Texture>> {
    static Node encode(const std::shared_ptr<Texture> &rhs) {
        Node node;
        if (std::shared_ptr<SolidColorTexture> p = std::dynamic_pointer_cast<SolidColorTexture>(rhs)) {
            return convert<std::shared_ptr<SolidColorTexture>>::encode(p);
        } else if (std::shared_ptr<CheckerTexture> p = std::dynamic_pointer_cast<CheckerTexture>(rhs)) {
            return convert<std::shared_ptr<CheckerTexture>>::encode(p);
        } else if (std::shared_ptr<ImageTexture> p = std::dynamic_pointer_cast<ImageTexture>(rhs)) {
            return convert<std::shared_ptr<ImageTexture>>::encode(p);
        } else if (std::shared_ptr<NoiseTexture> p = std::dynamic_pointer_cast<NoiseTexture>(rhs)) {
            return convert<std::shared_ptr<NoiseTexture>>::encode(p);
        }
        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> &refs, const std::shared_ptr<Texture> &rhs) {
        Node node;
        if (std::shared_ptr<SolidColorTexture> p = std::dynamic_pointer_cast<SolidColorTexture>(rhs)) {
            return convert<std::shared_ptr<SolidColorTexture>>::encode(p);
        } else if (std::shared_ptr<CheckerTexture> p = std::dynamic_pointer_cast<CheckerTexture>(rhs)) {
            return convert<std::shared_ptr<CheckerTexture>>::encode(refs, p);
        } else if (std::shared_ptr<ImageTexture> p = std::dynamic_pointer_cast<ImageTexture>(rhs)) {
            return convert<std::shared_ptr<ImageTexture>>::encode(p);
        } else if (std::shared_ptr<NoiseTexture> p = std::dynamic_pointer_cast<NoiseTexture>(rhs)) {
            return convert<std::shared_ptr<NoiseTexture>>::encode(p);
        }
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Texture> &rhs) {
        if (!node.IsMap()) return false;

        const std::string type = node["type"].as<std::string>();
        if (type == "solid_color") {
            rhs = node.as<std::shared_ptr<SolidColorTexture>>();
            return true;
        } else if (type == "checker") {
            rhs = node.as<std::shared_ptr<CheckerTexture>>();
            return true;
        } else if (type == "image") {
            rhs = node.as<std::shared_ptr<ImageTexture>>();
            return true;
        } else if (type == "noise") {
            rhs = node.as<std::shared_ptr<NoiseTexture>>();
            return true;
        }

        return false;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &refs, std::shared_ptr<Texture> &rhs) {
        if (!node.IsMap()) return false;

        if (node["type"].as<std::string>() == "solid_color") {
            rhs = node.as<std::shared_ptr<SolidColorTexture>>();
            return true;
        } else if(node["type"].as<std::string>() == "checker") {
            std::shared_ptr<CheckerTexture> p;
            if (convert<std::shared_ptr<CheckerTexture>>::decode(node, refs, p)) {
                rhs = p;
                return true;
            }
        } else if(node["type"].as<std::string>() == "image") {
            rhs = node.as<std::shared_ptr<ImageTexture>>();
            return true;
        } else if (node["type"].as<std::string>() == "noise") {
            rhs = node.as<std::shared_ptr<NoiseTexture>>();
            return true;
        }

        return false;
    }
};

template<>
struct convert<std::shared_ptr<Lambertian>> {
    static Node encode(const std::shared_ptr<Lambertian> &rhs) {
        Node node;
        node["type"] = Lambertian::NAME;
        node["texture"] = rhs->tex_;
        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> textures, std::shared_ptr<Lambertian> &rhs) {
        Node node;
        node["type"] = Lambertian::NAME;
        try {
            node["texture"] = textures.at(rhs->tex_);
        } catch (std::out_of_range &_) {
            node["texture"] = rhs->tex_;
        }
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Lambertian> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Lambertian::NAME) return false;

        std::shared_ptr<Texture> tex = node["texture"].as<std::shared_ptr<Texture>>();
        rhs = std::make_shared<Lambertian>(tex);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Material> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Lambertian::NAME) { return false; }

        if (node["texture"].IsScalar()) {
            rhs = std::make_shared<Lambertian>(textures.at(node["texture"].as<std::string>()));
        } else {
            rhs = std::make_shared<Lambertian>(node["texture"].as<std::shared_ptr<Texture>>());
        }

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Metal>> {
    static Node encode(const std::shared_ptr<Metal> &rhs) {
        Node node;
        node["type"] = Metal::NAME;
        node["color"] = rhs -> color_;
        node["fuzz"] = rhs ->fuzz_;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Metal> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Metal::NAME) return false;

        rhs = std::make_shared<Metal>(node["color"].as<Color>(), node["fuzz"].as<double>());

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Dielectric>> {
    static Node encode(const std::shared_ptr<Dielectric> &rhs) {
        Node node;
        node["type"] = Dielectric::NAME;
        node["refraction_index"] = rhs->refraction_index_;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Dielectric> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Dielectric::NAME) return false;

        rhs = std::make_shared<Dielectric>(node["refraction_index"].as<double>());

        return true;
    }
};

template<>
struct convert<std::shared_ptr<DiffuseLight>> {
    static Node encode(const std::shared_ptr<DiffuseLight> &rhs) {
        Node node;

        node["type"] = DiffuseLight::NAME;
        node["texture"] = rhs->tex_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures, const std::shared_ptr<DiffuseLight> &rhs) {
        Node node;

        node["type"] = DiffuseLight::NAME;
        try  {
            node["texture"] = textures.at(rhs->tex_);
        } catch (const std::out_of_range &e) {
            node["texture"] = rhs->tex_;
        }

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<DiffuseLight> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != DiffuseLight::NAME) return false;

        const auto tex = node["texture"].as<std::shared_ptr<Texture>>();
        rhs = std::make_shared<DiffuseLight>(tex);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<DiffuseLight> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != DiffuseLight::NAME) return false;

        std::shared_ptr<Texture> tex;
        if (node["texture"].IsScalar()) {
            tex = textures.at(node["texture"].as<std::string>());
        } else {
            tex = node["texture"].as<std::shared_ptr<Texture>>();
        }

        rhs = std::make_shared<DiffuseLight>(tex);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Isotropic>> {
    static Node encode(const std::shared_ptr<Isotropic> &rhs) {
        Node node;

        node["type"] = Isotropic::NAME;
        node["texture"] = rhs->tex_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures, const std::shared_ptr<Isotropic> &rhs) {
        Node node;

        node["type"] = Isotropic::NAME;
        try {
            node["texture"] = textures.at(rhs->tex_);
        } catch (const std::out_of_range &e) {
            node["texture"] = rhs->tex_;
        }

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Isotropic> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Isotropic::NAME) return false;

        const auto tex = node["texture"].as<std::shared_ptr<Texture>>();

        rhs = std::make_shared<Isotropic>(tex);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Isotropic> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != Isotropic::NAME) return false;

        std::shared_ptr<Texture> tex;
        if (node["texture"].IsScalar()) {
            tex = textures.at(node["texture"].as<std::string>());
        } else {
            tex = node["texture"].as<std::shared_ptr<Texture>>();
        }

        rhs = std::make_shared<Isotropic>(tex);

        return true;
    }
};


template<>
struct convert<std::shared_ptr<Material>> {
    static Node encode(const std::shared_ptr<Material> &rhs) {
        Node node;

        if (std::shared_ptr<Lambertian> p = std::dynamic_pointer_cast<Lambertian>(rhs)) {
            return convert<std::shared_ptr<Lambertian>>::encode(p);
        } else if (std::shared_ptr<Metal> p = std::dynamic_pointer_cast<Metal>(rhs)){
            return convert<std::shared_ptr<Metal>>::encode(p);
        } else if (std::shared_ptr<Dielectric> p = std::dynamic_pointer_cast<Dielectric>(rhs)){
            return convert<std::shared_ptr<Dielectric>>::encode(p);
        } else if (std::shared_ptr<DiffuseLight> p = std::dynamic_pointer_cast<DiffuseLight>(rhs)){
            return convert<std::shared_ptr<DiffuseLight>>::encode(p);
        } else if (std::shared_ptr<Isotropic> p = std::dynamic_pointer_cast<Isotropic>(rhs)){
            return convert<std::shared_ptr<Isotropic>>::encode(p);
        }

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures, const std::shared_ptr<Material> &rhs) {
        Node node;

        if (std::shared_ptr<Lambertian> p = std::dynamic_pointer_cast<Lambertian>(rhs)) {
            return convert<std::shared_ptr<Lambertian>>::encode(textures, p);
        } else if (std::shared_ptr<Metal> p = std::dynamic_pointer_cast<Metal>(rhs)){
            return convert<std::shared_ptr<Metal>>::encode(p);
        } else if (std::shared_ptr<Dielectric> p = std::dynamic_pointer_cast<Dielectric>(rhs)){
            return convert<std::shared_ptr<Dielectric>>::encode(p);
        } else if (std::shared_ptr<DiffuseLight> p = std::dynamic_pointer_cast<DiffuseLight>(rhs)){
            return convert<std::shared_ptr<DiffuseLight>>::encode(textures, p);
        } else if (std::shared_ptr<Isotropic> p = std::dynamic_pointer_cast<Isotropic>(rhs)){
            return convert<std::shared_ptr<Isotropic>>::encode(textures, p);
        }

        return node;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Material> &rhs) {
        if (!node.IsMap()) { return false; }

        const std::string type = node["type"].as<std::string>();

        if (type == Lambertian::NAME) {
            return convert<std::shared_ptr<Lambertian>>::decode(node, textures, rhs);
        } else if (type == Metal::NAME) {
            rhs = node.as<std::shared_ptr<Metal>>();
            return true;
        } else if (type == Dielectric::NAME) {
            rhs = node.as<std::shared_ptr<Dielectric>>();
            return true;
        } else if (type == DiffuseLight::NAME) {
            std::shared_ptr<DiffuseLight> p;
            if (convert<std::shared_ptr<DiffuseLight>>::decode(node, textures, p)) {
                rhs = p;
                return true;
            }
        } else if (type == Isotropic::NAME) {
            std::shared_ptr<Isotropic> p;
            if (convert<std::shared_ptr<Isotropic>>::decode(node, textures, p)) {
                rhs = p;
                return true;
            }
        }

        return false;
    }

    static bool decode(const Node &node, std::shared_ptr<Material> &rhs) {
        const std::string type = node["type"].as<std::string>();
        if (type == Lambertian::NAME) {
            rhs = node.as<std::shared_ptr<Lambertian>>();
            return true;
        } else if (type == Metal::NAME) {
            rhs = node.as<std::shared_ptr<Metal>>();
            return true;
        } else if (type == Dielectric::NAME) {
            rhs = node.as<std::shared_ptr<Dielectric>>();
            return true;
        } else if (type == DiffuseLight::NAME) {
            rhs = node.as<std::shared_ptr<DiffuseLight>>();
            return true;
        } else if (type == Isotropic::NAME) {
            rhs = node.as<std::shared_ptr<Isotropic>>();
            return true;
        } 

        return false;
    }

};

template<typename T>
struct convert<Point3<T>> {
    static Node encode(const Point3<T> &rhs) {
        Node node;

        node.push_back(rhs.x());
        node.push_back(rhs.y());
        node.push_back(rhs.z());

        return node;
    }

    static bool decode(const Node &node, Point3<T> &rhs) {
        if (!node.IsSequence() || node.size() != 3)  return false;

        rhs.elem[0] = node[0].as<double>();
        rhs.elem[1] = node[1].as<double>();
        rhs.elem[2] = node[2].as<double>();

        return true;
    }
};

template<typename T>
struct convert<Ray<T>> {
    static Node encode(const Ray<T> &rhs) {
        Node node;
        node["origin"] = rhs.origin();
        node["direction"] = rhs.direction();
        return node;
    }

    static bool decode(const Node &node, Ray<T> &rhs) {
        if (!node.IsMap()) return false;

        rhs.origin_ = node["origin"].as<Point3<T>>();
        rhs.direction_ = node["direction"].as<Vec3<T>>();

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Sphere>> {
    static Node encode(const std::shared_ptr<Sphere> &rhs) {
        Node node;

        node["type"] = "sphere";
        node["origin"] = rhs->origin_;
        node["radius"] = rhs->radius_;
        node["material"] = rhs->mat_;
        
        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures, const std::shared_ptr<Sphere> &rhs) {
        Node node;

        node["type"] = "sphere";
        node["origin"] = rhs->origin_;
        node["radius"] = rhs->radius_;
        try {
            node["material"] = materials.at(rhs->mat_);
        } catch (std::out_of_range &_) {
            node["material"] = convert<std::shared_ptr<Material>>::encode(textures, rhs->mat_);
        }
        
        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Sphere> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "sphere") return false;

        const auto origin = node["origin"].as<Ray<double>>();
        const auto radius = node["radius"].as<double>();
        const auto mat = node["material"].as<std::shared_ptr<Material>>();

        rhs = std::make_shared<Sphere>(origin, radius, mat);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures,std::shared_ptr<Sphere> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "sphere") return false;

        const auto origin = node["origin"].as<Ray<double>>();
        const auto radius = node["radius"].as<double>();

        std::shared_ptr<Material> mat;
        if (node["material"].IsScalar()) {
            mat = materials.at(node["material"].as<std::string>());
        } else if (!convert<std::shared_ptr<Material>>::decode(node["material"], textures, mat)) {
            return false;
        }
        rhs = std::make_shared<Sphere>(origin, radius, mat);
        
        return true;
    }
};

template<>
struct convert<std::shared_ptr<Quad>> {
    static Node encode(const std::shared_ptr<Quad> &rhs) {
        Node node;

        node["type"] = "quad";
        node["origin"] = rhs->origin_;
        node["u"] = rhs->u_;
        node["v"] = rhs->v_;
        node["material"] = rhs->mat_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures, std::shared_ptr<Quad> &rhs) {
        Node node;

        node["type"] = "quad";
        node["origin"] = rhs->origin_;
        node["u"] = rhs->u_;
        node["v"] = rhs->v_;
        try {
            node["material"] = materials.at(rhs->mat_);
        } catch (const std::out_of_range &e) {
            node["material"] = convert<std::shared_ptr<Material>>::encode(textures, rhs->mat_);
        }

        return node;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Quad> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "quad") return false;

        const auto origin = node["origin"].as<Point3<double>>();
        const auto u = node["u"].as<Vec3<double>>();
        const auto v = node["v"].as<Vec3<double>>();
        std::shared_ptr<Material> mat;
        if (node["material"].IsScalar()) {
            mat = materials.at(node["material"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Material>>::decode(node["material"], textures, mat)) {
                return false;
            }
        }

        rhs = std::make_shared<Quad>(origin, u, v, mat);

        return true;
    }

    static bool decode(const Node &node, std::shared_ptr<Quad> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "quad") return false;

        const auto origin = node["origin"].as<Point3<double>>();
        const auto u = node["u"].as<Vec3<double>>();
        const auto v = node["v"].as<Vec3<double>>();
        const auto mat = node["material"].as<std::shared_ptr<Material>>();

        rhs = std::make_shared<Quad>(origin, u, v, mat);

        return true;
    }
};


template<>
struct convert<std::shared_ptr<Hittable>> {
    static Node encode(const std::shared_ptr<Hittable> &rhs);
    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<Hittable> &rhs);
    static bool decode(const Node &node, std::shared_ptr<Hittable> &rhs);
    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Hittable> &rhs);
};

template<>
struct convert<std::shared_ptr<BVHNode>> {
    static Node encode(const std::shared_ptr<BVHNode> &rhs) {
        Node node;

        node["type"] = BVHNode::NAME;
        node["left"] = rhs->left_;
        node["right"] = rhs->left_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<BVHNode> &rhs) {
        Node node;

        node["type"] = BVHNode::NAME;

        try {
            node["left"] = refs.at(rhs->left_);
        } catch (const std::out_of_range &e) {
            node["left"] = convert<std::shared_ptr<Hittable>>::encode(refs, materials, textures, rhs->left_);
        }

        try {
            node["right"] = refs.at(rhs->right_);
        } catch (const std::out_of_range &e) {
            node["right"] = convert<std::shared_ptr<Hittable>>::encode(refs, materials, textures, rhs->right_);
        }

        return node;
    }
    static bool decode(const Node &node, std::shared_ptr<BVHNode> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != BVHNode::NAME) return false;

        const auto left = node["left"].as<std::shared_ptr<Hittable>>();
        const auto right = node["right"].as<std::shared_ptr<Hittable>>();

        rhs = std::make_shared<BVHNode>(left, right);

        return true;
    }
    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<BVHNode> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != BVHNode::NAME) return false;

        std::shared_ptr<Hittable> left;
        if (node["left"].IsScalar()) {
            left = refs.at(node["left"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Hittable>>::decode(node["left"], refs, materials, textures, left)) {
                return false;
            }
        }

        std::shared_ptr<Hittable> right;
        if (node["right"].IsScalar()) {
            right = refs.at(node["right"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Hittable>>::decode(node["right"], refs, materials, textures, right)) {
                return false;
            }
        }

        rhs = std::make_shared<BVHNode>(left, right);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<HittableList>> {
    static Node encode(const std::shared_ptr<HittableList> &rhs) {
        Node node;

        node["type"] = HittableList::NAME;
        Node objs_node = Node();
        for (const auto &obj: rhs->objs) {
            objs_node.push_back(obj);
        }
        node["objects"] = objs_node;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<HittableList> &rhs) {
        Node node;

        node["type"] = HittableList::NAME;
        Node objs_node = Node();
        for (const auto &obj : rhs->objs) {
            try {
                objs_node.push_back(refs.at(obj));
            } catch (const std::out_of_range &e) {
                objs_node.push_back(obj);
            }
        }
        node["objects"] = objs_node;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<HittableList> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != HittableList::NAME) return false;

        rhs = std::make_shared<HittableList>();
        if (node["objects"].IsDefined() && node["objects"].IsSequence()) {
            for (const auto obj_node : node["objects"]) {
                rhs->add(obj_node.as<std::shared_ptr<Hittable>>());
            }
        }

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<HittableList> &rhs){
        if (!node.IsMap() || node["type"].as<std::string>() != HittableList::NAME) return false;

        rhs = std::make_shared<HittableList>();
        if (node["objects"].IsDefined() && node["objects"].IsSequence()) {
            for (const auto obj_node : node["objects"]) {
                if (obj_node.IsScalar()) {
                    rhs->add(refs.at(obj_node.as<std::string>()));
                } else {
                    std::shared_ptr<Hittable> p;
                    if (convert<std::shared_ptr<Hittable>>::decode(obj_node, refs, materials, textures, p)) {
                        rhs->add(p);
                    } else {
                        return false;
                    }
                }
            }
        }

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Box>> {
    static Node encode(const std::shared_ptr<Box> &rhs) {
        Node node;

        node["type"] = "box";
        node["a"] = rhs->a_;
        node["b"] = rhs->b_;
        node["material"] = rhs->mat_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<Box> &rhs) {
        Node node;
        node["type"] = "box";
        node["a"] = rhs->a_;
        node["b"] = rhs->b_;

        try {
            node["material"] = materials.at(rhs->mat_);
        } catch (const std::out_of_range &e) {
            node["material"] = convert<std::shared_ptr<Material>>::encode(textures, rhs->mat_);
        }

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Box> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "box") return false;

        std::shared_ptr<Material> mat;
        if (!convert<std::shared_ptr<Material>>::decode(node["material"], mat)) {
            return false;
        }

        const auto a = node["a"].as<Point3<double>>();
        const auto b = node["b"].as<Point3<double>>();

        rhs = std::make_shared<Box>(a, b, mat);

        return true;
    }


    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Box> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "box") return false;

        std::shared_ptr<Material> mat;
        if (node["material"].IsScalar()) {
            mat = materials.at(node["material"].as<std::string>());
        } else if (!convert<std::shared_ptr<Material>>::decode(node["material"], textures, mat)) {
            return false;
        }

        const auto a = node["a"].as<Point3<double>>();
        const auto b = node["b"].as<Point3<double>>();

        rhs = std::make_shared<Box>(a, b, mat);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<Translate>> {
    static Node encode(const std::shared_ptr<Translate> &rhs) {
        Node node;

        node["type"] = "translate";
        node["object"] = rhs->object_;
        node["offset"] = rhs->offset_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<Translate> &rhs) {
        Node node;

        node["type"] = "translate";
        try {
            node["object"] = refs.at(rhs->object_);
        } catch (const std::out_of_range &e) {
            node["object"] = convert<std::shared_ptr<Hittable>>::encode(refs, materials, textures, rhs->object_);
        }
        node["offset"] = rhs->offset_;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<Translate> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "translate") return false;

        std::shared_ptr<Hittable> object;
        if (!convert<std::shared_ptr<Hittable>>::decode(node["object"], object)) {
            return false;
        }

        const auto offset = node["offset"].as<Vec3<double>>();

        rhs = std::make_shared<Translate>(object, offset);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Translate> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "translate") return false;

        std::shared_ptr<Hittable> object;
        if (node["object"].IsScalar()) {
            object = refs.at(node["object"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Hittable>>::decode(node["object"], refs, materials, textures, object)) {
                return false;
            }
        }

        const auto offset = node["offset"].as<Vec3<double>>();

        rhs = std::make_shared<Translate>(object, offset);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<RotateY>> {
    static Node encode(const std::shared_ptr<RotateY> &rhs) {
        Node node;

        node["type"] = "rotate-y";
        node["object"] = rhs->object_;
        node["angle"] = radians_to_degrees(rhs->theta_);

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<RotateY> &rhs) {
        Node node;

        node["type"] = "rotate-y";
        try {
            node["object"] = refs.at(rhs->object_);
        } catch (const std::out_of_range &e) {
            node["object"] = convert<std::shared_ptr<Hittable>>::encode(refs, materials, textures, rhs->object_);
        }

        node["angle"] = radians_to_degrees(rhs->theta_);

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<RotateY> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "rotate-y") return false;

        std::shared_ptr<Hittable> object;
        if (!convert<std::shared_ptr<Hittable>>::decode(node["object"], object)) {
            return false;
        }

        const auto angle = node["angle"].as<double>();

        rhs = std::make_shared<RotateY>(object, angle);

        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<RotateY> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != "rotate-y") return false;

        std::shared_ptr<Hittable> object;
        if (node["object"].IsScalar()) {
            object = refs.at(node["object"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Hittable>>::decode(node["object"], refs, materials, textures, object)) {
                return false;
            }
        }

        const auto angle = node["angle"].as<double>();

        rhs = std::make_shared<RotateY>(object, angle);

        return true;
    }
};

template<>
struct convert<std::shared_ptr<ConstantMedium>> {
    static Node encode(const std::shared_ptr<ConstantMedium> &rhs) {
        Node node;

        node["type"] = ConstantMedium::NAME;
        node["boundary"] = rhs->boundary_;
        node["density"] = -1 / rhs->neg_inv_density_;
        node["phase_function"] = rhs->phase_function_;

        return node;
    }

    static Node encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<ConstantMedium> &rhs) {
        Node node;

        node["type"] = ConstantMedium::NAME;
        try {
            node["boundary"] = refs.at(rhs->boundary_);
        } catch (const std::out_of_range &e) {
            node["boundary"] = rhs->boundary_;
        }
        node["density"] = -1 / rhs->neg_inv_density_;
        node["phase_function"] = rhs->phase_function_;

        return node;
    }

    static bool decode(const Node &node, std::shared_ptr<ConstantMedium> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != ConstantMedium::NAME) return false;

        std::shared_ptr<Hittable> boundary;
        if (!convert<std::shared_ptr<Hittable>>::decode(node["boundary"], boundary)) {
            return false;
        }

        const auto density = node["density"].as<double>();

        std::shared_ptr<Material> phase_function;
        if (!convert<std::shared_ptr<Material>>::decode(node["phase_function"], phase_function)) {
            return false;
        }

        rhs = std::make_shared<ConstantMedium>(boundary, density, phase_function);
 
        return true;
    }

    static bool decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<ConstantMedium> &rhs) {
        if (!node.IsMap() || node["type"].as<std::string>() != ConstantMedium::NAME) return false;

        std::shared_ptr<Hittable> boundary;
        if (node["boundary"].IsScalar()) {
            boundary = refs.at(node["boundary"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Hittable>>::decode(node["boundary"], refs, materials, textures, boundary)) {
                return false;
            }
        }
        const auto density = node["density"].as<double>();
        std::shared_ptr<Material> phase_function;
        if (node["phase_function"].IsScalar()) {
            phase_function = materials.at(node["phase_function"].as<std::string>());
        } else {
            if (!convert<std::shared_ptr<Material>>::decode(node["phase_function"], textures, phase_function)) {
                return false;
            }
        }

        rhs = std::make_shared<ConstantMedium>(boundary, density, phase_function);
 
        return true;
    }
};


template<>
struct convert<Scene> {
    static Node encode(const Scene &rhs) {
        Node node;

        // Create Name-Maps
        std::unordered_map<std::shared_ptr<Texture>, std::string> tex_name_map;
        for (size_t i = 0; i < rhs.textures_.size(); i++) {
            tex_name_map[rhs.textures_[i]] = std::format("Texture-{}", i);
        }

        std::unordered_map<std::shared_ptr<Material>, std::string> mat_name_map;
        for (size_t i = 0; i < rhs.materials_.size(); i++) {
            mat_name_map[rhs.materials_[i]] = std::format("Material-{}", i);
        }

        std::unordered_map<std::shared_ptr<Hittable>, std::string> ref_name_map;
        for (size_t i = 0; i < rhs.refs_.size(); i++) {
            ref_name_map[rhs.refs_[i]] = std::format("Ref-{}", i);
        }
        for (size_t i = 0; i < rhs.objs_.objs.size(); i++) {
            ref_name_map[rhs.objs_.objs[i]] = std::format("Obj-{}", i);
        }

        {
            // Encode Textures
            node["textures"] = Node();
            for (const auto &tex : rhs.textures_) {
                const auto &name = tex_name_map[tex];
                Node tex_node = convert<std::shared_ptr<Texture>>::encode(tex_name_map, tex);
                tex_node["name"] = name;
                node["textures"].push_back(tex_node);
            }
        }

        {
            // Encode Materials
            node["materials"] = Node();
            for (const auto& mat : rhs.materials_) {
                const auto &name = mat_name_map[mat];

                Node mat_node = convert<std::shared_ptr<Material>>::encode(tex_name_map, mat);
                mat_node["name"] = name;
                node["materials"].push_back(mat_node);
            }
        }

        {
            // Encode Refs
            node["refs"] = Node();
            for (const auto &ref : rhs.refs_) {
                const auto &name = ref_name_map[ref];
                Node ref_node = convert<std::shared_ptr<Hittable>>::encode(ref_name_map, mat_name_map, tex_name_map, ref);
                ref_node["name"] = name;
                node["refs"].push_back(ref_node);
            }
        }

        {
            // Encode Objects
            node["objects"] = Node();
            for (const auto &obj : rhs.objs_.objs) {
                const auto &name = ref_name_map[obj];
                Node obj_node = convert<std::shared_ptr<Hittable>>::encode(ref_name_map, mat_name_map, tex_name_map, obj);
                obj_node["name"] = name;
                node["objects"].push_back(obj_node);
            }
        }

        node["camera"] = rhs.cb_;

        return node;
    }

    static bool decode(const Node &node, Scene &rhs) {
        if (!node.IsMap()) { return false; }
        
        // Create Name-Maps
        std::unordered_map<std::string, std::shared_ptr<Texture>> tex_map;
        std::unordered_map<std::string, std::shared_ptr<Material>> mat_map;
        std::unordered_map<std::string, std::shared_ptr<Hittable>> ref_map;

        Node textures_node = node["textures"];
        if (textures_node.IsDefined() && textures_node.IsSequence()) {
            for (const auto &tex_node : textures_node) {
                std::string name = tex_node["name"].as<std::string>();
                std::shared_ptr<Texture> tex;
                if (convert<std::shared_ptr<Texture>>::decode(tex_node, tex_map, tex)) {
                    rhs.add_texture(tex);
                    tex_map[name] = tex;
                } else {
                    return false;
                }
            }
        }

        Node materials_node = node["materials"];
        if (materials_node.IsDefined() && materials_node.IsSequence()) {
            for (const auto &mat_node : materials_node) {
                std::string name = mat_node["name"].as<std::string>();
                std::shared_ptr<Material> mat;
                if (convert<std::shared_ptr<Material>>::decode(mat_node, tex_map, mat)) {
                    rhs.add_material(mat);
                    mat_map[name] = mat;
                } else {
                    return false;
                }
            }
        }

        Node refs_node = node["refs"];
        if (refs_node.IsDefined() && refs_node.IsSequence()) {
            for (const auto &ref_node: refs_node) {
                std::string name = ref_node["name"].as<std::string>();
                std::shared_ptr<Hittable> ref;
                if (convert<std::shared_ptr<Hittable>>::decode(ref_node, ref_map, mat_map, tex_map, ref)) {
                    rhs.add_ref(ref);
                    ref_map[name] = ref;
                } else {
                    return false;
                }
            }
        }

        Node objects_node = node["objects"];
        if (objects_node.IsDefined() && objects_node.IsSequence()) {
            for (const auto &obj_node : objects_node) {
                std::shared_ptr<Hittable> obj;
                if (convert<std::shared_ptr<Hittable>>::decode(obj_node, ref_map, mat_map, tex_map, obj)) {
                    rhs.add_object(obj);
                    if (obj_node["name"].IsDefined()) {
                        ref_map[obj_node["name"].as<std::string>()] = obj;
                    }
                } else {
                    return false;
                }
            }
        }

        Node camera_node = node["camera"];
        if (camera_node.IsDefined()) {
            rhs.cb_ = camera_node.as<CameraBuilder>();
        }

        return true;
    }
};

template<>
struct convert<CameraBuilder> {
    static Node encode(const CameraBuilder &rhs) {
        Node node;

        node["look_at"] = rhs.lookat_;
        node["look_from"] = rhs.lookfrom_;
        node["vup"] = rhs.vup_;
        node["vfov"] = rhs.vfov_;
        node["focus_dist"] = rhs.focus_dist_;
        node["defocus_angle"] = rhs.defocus_angle_;
        node["background"] = rhs.background_;

        return node;
    }

    static bool decode(const Node &node, CameraBuilder &rhs) {
        if (!node.IsMap()) return false;

        
        if (node["look_at"].IsDefined()) {
            rhs.lookat_ = node["look_at"].as<Point3<double>>();
        }

        if (node["look_from"].IsDefined()) {
            rhs.lookfrom_ = node["look_from"].as<Point3<double>>();
        }

        if (node["vup"].IsDefined()) {
            rhs.vup_ = node["vup"].as<Vec3<double>>();
        }

        if (node["vfov"].IsDefined()) {
            rhs.vfov_ = node["vfov"].as<double>();
        }

        if (node["focus_dist"].IsDefined()) {
            rhs.focus_dist_ = node["focus_dist"].as<double>();
        }

        if (node["defocus_angle"].IsDefined()) {
            rhs.defocus_angle_ = node["defocus_angle"].as<double>();
        }

        if (node["background"].IsDefined()) {
            rhs.background_ = node["background"].as<Color>();
        }

        return true;
    }
};

template<>
struct convert<RenderSettings> {
    static Node encode(const RenderSettings &rhs) {
        Node node;

        node["threads"] = rhs.num_threads;
        node["max_depth"] = rhs.max_depth_;
        node["samples_per_pixel"] = rhs.samples_per_pixel_;
        node["chunk_width"] = rhs.chunk_width_;
        node["chunk_height"] = rhs.chunk_height_;

        return node;
    }

    static bool decode(const Node &node, RenderSettings &rhs) {
        if (!node.IsMap()) return false;

        if (node["threads"].IsDefined()) {
            rhs.num_threads = node["threads"].as<uint32_t>();
        }
        
        if (node["max_depth"].IsDefined()) {
            rhs.max_depth_ = node["max_depth"].as<int32_t>();
        }

        if (node["samples_per_pixel"].IsDefined()) {
            rhs.set_samples_per_pixel(node["samples_per_pixel"].as<int32_t>());
        }

        if (node["chunk_width"].IsDefined()) {
            rhs.chunk_width_ = node["chunk_width"].as<int32_t>();
        }

        if (node["chunk_height"].IsDefined()) {
            rhs.chunk_height_ = node["chunk_height"].as<int32_t>();
        }

        return true;
    }
};
};

inline std::ostream& operator<<(std::ostream& out, const Scene& scene) {
    YAML::Emitter em;
    em << YAML::convert<Scene>::encode(scene);

    out << em.c_str();

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const CameraBuilder& cb) {
    YAML::Emitter em;
    em << YAML::convert<CameraBuilder>::encode(cb);

    out << em.c_str();

    return out;
}

inline std::ostream& operator<<(std::ostream& out, const RenderSettings& rs) {
    YAML::Emitter em;
    em << YAML::convert<RenderSettings>::encode(rs);

    out << em.c_str();

    return out;
}


inline Scene LoadScene(const std::string &file_name) {
    YAML::Node node = YAML::LoadFile(file_name);
    return node.as<Scene>();
}

inline RenderSettings LoadRenderSettings(const std::string &file_name) {
    YAML::Node node = YAML::LoadFile(file_name);
    return node.as<RenderSettings>();
}

inline CameraBuilder LoadCamera(const std::string &file_name) {
    YAML::Node node = YAML::LoadFile(file_name);
    return node.as<CameraBuilder>();
}
