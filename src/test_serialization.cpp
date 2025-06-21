#include <cassert>
#include <cstring>
#include <memory>
#include "material.hpp"
#include "scene.hpp"
#include "serialization.hpp"
#include "texture.hpp"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/parse.h"

void test_vec3() {
    {
        YAML::Node node = YAML::Load("[1, 2, 3]");
        const Vec3<double> v = node.as<Vec3<double>>();

        assert(v.x() == 1.0);
        assert(v.y() == 2.0);
        assert(v.z() == 3.0);
    }

    { 
        const Vec3<double> v(1.0, 2.0, 3.0);

        YAML::Node node = YAML::convert<Vec3<double>>::encode(v);
        YAML::Emitter out;
        out << YAML::Flow << node;

        const char *str = out.c_str();
        const char *expected = "[1, 2, 3]";
        assert(strncmp(str, expected, strlen(expected)) == 0);
    }
}

void test_texture() {
    {
        // SolidColorTexture Decoding
        const std::string encoding = "{type: solid_color, color: [0.25, 0.33, 0.5]}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<SolidColorTexture> tex = decoding_node.as<std::shared_ptr<SolidColorTexture>>();

        // SolidColorTexture Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<SolidColorTexture>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }

    {
        // CheckerTexture Decoding
        const std::string encoding = "{type: checker, scale: 0.1, even: {type: solid_color, color: [0.25, 0.25, 0.25]}, odd: {type: solid_color, color: [0.75, 0.75, 0.75]}}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<CheckerTexture> tex = decoding_node.as<std::shared_ptr<CheckerTexture>>();

        // CheckerTexture Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<CheckerTexture>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }

    {
        // ImageTexture Decoding
        const std::string encoding = "{type: image, file_name: images/earthmap.jpg}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<ImageTexture> tex = decoding_node.as<std::shared_ptr<ImageTexture>>();

        // ImageTexture Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<ImageTexture>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }
}

void test_material() {
    {
        // Lambertian Decoding
        const std::string encoding = "{type: lambertian, texture: {type: solid_color, color: [0.25, 0.33, 0.5]}}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<Lambertian> tex = decoding_node.as<std::shared_ptr<Lambertian>>();

        // Lambertian Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<Lambertian>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }

    {
        // Metal Decoding
        const std::string encoding = "{type: metal, color: [0.1, 0.2, 0.3], fuzz: 0.11}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<Metal> tex = decoding_node.as<std::shared_ptr<Metal>>();

        // Metal Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<Metal>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }

    {
        // Dielectric Decoding
        const std::string encoding = "{type: dielectric, refraction_index: 0.5}";
        YAML::Node decoding_node = YAML::Load(encoding);
        std::shared_ptr<Dielectric> tex = decoding_node.as<std::shared_ptr<Dielectric>>();

        // Dielectric Encoding
        YAML::Node encoding_node = YAML::convert<std::shared_ptr<Dielectric>>::encode(tex);
        YAML::Emitter out;
        out << YAML::Flow << encoding_node;

        const char *str = out.c_str();
        assert(std::strncmp(str, encoding.c_str(), encoding.size()) == 0);
    }
}

void test_scene() {
    {
        Scene scene;
        const auto solid_color_1 = std::make_shared<SolidColorTexture>(Color(0.1, 0.2, 0.3));
        const auto solid_color_2 = std::make_shared<SolidColorTexture>(Color(0.9, 0.8, 0.7));

        scene.add_texture(solid_color_1);
        scene.add_texture(solid_color_2);
        scene.add_texture(std::make_shared<CheckerTexture>(0.1, solid_color_1, solid_color_2));

        scene.add_material(std::make_shared<Lambertian>(solid_color_1));
        const auto metal = std::make_shared<Metal>(Color(0.1, 0.2, 0.3), 0.1);
        scene.add_material(metal);
        scene.add_material(std::make_shared<Dielectric>(0.5));

        scene.add_object(std::make_shared<Sphere>(Point3<double>(), Point3<double>(0, 0, 1), 10.0, metal));

        YAML::Node encoding_node = YAML::convert<Scene>::encode(scene);
        YAML::Emitter out;
        out << YAML::Flow;
        out << encoding_node;

        std::cout << out.c_str() << '\n';

        // Decoding
        YAML::Node decoding_node = YAML::Load("{textures: [{type: solid_color, color: [0.1, 0.2, 0.3], name: SolidTexture1}, {type: solid_color, color: [0.9, 0.8, 0.7], name: SolidTexture2}, {type: checker, scale: 0.1, even: SolidTexture1, odd: SolidTexture2, name: CheckerTexture1}], materials: [{type: dielectric, refraction_index: 0.5, name: Dielectric1}, {type: metal, color: [0.1, 0.2, 0.3], fuzz: 0.1, name: Metal1}, {type: lambertian, texture: SolidTexture1, name: Lambertian1}], objects: [{type: sphere, origin: {origin: [0, 0, 0], direction: [0, 0, 1]}, radius: 10, material: Metal1}]}");
        Scene scene_2 = decoding_node.as<Scene>();
        scene_2.add_texture(std::make_shared<SolidColorTexture>(Color(0.5, 0.5, 0.5)));

        YAML::Node encoding_node_2 = YAML::convert<Scene>::encode(scene_2);
        YAML::Emitter out_2;
        out_2 << encoding_node_2;

        std::cout << out_2.c_str() << '\n';
    }
}

int main() {
    test_vec3();
    test_texture();
    test_material();
    test_scene();
}
