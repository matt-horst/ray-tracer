#pragma once
#include <memory>
#include "camera.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "bvh.hpp"

class Scene {
public:
    Scene() {}

    void add_texture(std::shared_ptr<Texture> tex) { textures_.push_back(tex); }
    void add_material(std::shared_ptr<Material> mat) { materials_.push_back(mat); }
    void add_ref(std::shared_ptr<Hittable> ref) { refs_.push_back(ref); }
    void add_object(std::shared_ptr<Hittable> obj) { objs_.add(obj); }

    std::shared_ptr<BVHNode> bvh() { return std::make_shared<BVHNode>(objs_); }
 
    std::vector<std::shared_ptr<Texture>> textures_;
    std::vector<std::shared_ptr<Material>> materials_;
    std::vector<std::shared_ptr<Hittable>> refs_;
    HittableList objs_;

    Camera camera(const Image &img) const { return cb_.build(img); }

    CameraBuilder cb_;
};

