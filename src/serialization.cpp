#include "serialization.hpp"
#include "bvh.hpp"
#include "constant_medium.hpp"
#include "hittable_list.hpp"
#include <memory>
#include <unordered_map>

namespace YAML {
Node convert<std::shared_ptr<Hittable>>::encode(const std::shared_ptr<Hittable> &rhs) {
    Node node;
    if (std::shared_ptr<Sphere> p = std::dynamic_pointer_cast<Sphere>(rhs)) {
        return convert<std::shared_ptr<Sphere>>::encode(p);
    } else if (std::shared_ptr<Quad> p = std::dynamic_pointer_cast<Quad>(rhs)) {
        return convert<std::shared_ptr<Quad>>::encode(p);
    } else if (std::shared_ptr<Box> p = std::dynamic_pointer_cast<Box>(rhs)) {
        return convert<std::shared_ptr<Box>>::encode(p);
    } else if (std::shared_ptr<Translate> p = std::dynamic_pointer_cast<Translate>(rhs)) {
        return convert<std::shared_ptr<Translate>>::encode(p);
    } else if (std::shared_ptr<RotateY> p = std::dynamic_pointer_cast<RotateY>(rhs)) {
        return convert<std::shared_ptr<RotateY>>::encode(p);
    } else if (std::shared_ptr<ConstantMedium> p = std::dynamic_pointer_cast<ConstantMedium>(rhs)) {
        return convert<std::shared_ptr<ConstantMedium>>::encode(p);
    } else if (std::shared_ptr<HittableList> p = std::dynamic_pointer_cast<HittableList>(rhs)) {
        return convert<std::shared_ptr<HittableList>>::encode(p);
    } else if (std::shared_ptr<BVHNode> p = std::dynamic_pointer_cast<BVHNode>(rhs)) {
        return convert<std::shared_ptr<BVHNode>>::encode(p);
    } 
    return node;
}

Node convert<std::shared_ptr<Hittable>>::encode(const std::unordered_map<std::shared_ptr<Hittable>, std::string> &refs, const std::unordered_map<std::shared_ptr<Material>, std::string> &materials, const std::unordered_map<std::shared_ptr<Texture>, std::string> &textures,  const std::shared_ptr<Hittable> &rhs) {
    Node node;
    if (std::shared_ptr<Sphere> p = std::dynamic_pointer_cast<Sphere>(rhs)) {
        return convert<std::shared_ptr<Sphere>>::encode(materials, textures, p);
    } else if (std::shared_ptr<Quad> p = std::dynamic_pointer_cast<Quad>(rhs)) {
        return convert<std::shared_ptr<Quad>>::encode(materials, textures, p);
    } else if (std::shared_ptr<Box> p = std::dynamic_pointer_cast<Box>(rhs)) {
        return convert<std::shared_ptr<Box>>::encode(materials, textures, p);
    } else if (std::shared_ptr<Translate> p = std::dynamic_pointer_cast<Translate>(rhs)) {
        return convert<std::shared_ptr<Translate>>::encode(refs, materials, textures, p);
    } else if (std::shared_ptr<RotateY> p = std::dynamic_pointer_cast<RotateY>(rhs)) {
        return convert<std::shared_ptr<RotateY>>::encode(refs, materials, textures, p);
    } else if (std::shared_ptr<ConstantMedium> p = std::dynamic_pointer_cast<ConstantMedium>(rhs)) {
        return convert<std::shared_ptr<ConstantMedium>>::encode(refs, materials, textures, p);
    } else if (std::shared_ptr<HittableList> p = std::dynamic_pointer_cast<HittableList>(rhs)) {
        return convert<std::shared_ptr<HittableList>>::encode(refs, materials, textures, p);
    } else if (std::shared_ptr<BVHNode> p = std::dynamic_pointer_cast<BVHNode>(rhs)) {
        return convert<std::shared_ptr<BVHNode>>::encode(refs, materials, textures, p);
    } 
    return node;
}

bool convert<std::shared_ptr<Hittable>>::decode(const Node &node, std::shared_ptr<Hittable> &rhs) {
    if (!node.IsMap()) return false;

    const auto type = node["type"].as<std::string>();
    if (type == "sphere") {
        std::shared_ptr<Sphere> p;
        if (convert<std::shared_ptr<Sphere>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "quad") {
        std::shared_ptr<Quad> p;
        if (convert<std::shared_ptr<Quad>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "translate") {
        std::shared_ptr<Translate> p;
        if (convert<std::shared_ptr<Translate>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "rotate-y") {
        std::shared_ptr<RotateY> p;
        if (convert<std::shared_ptr<RotateY>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "box") {
        std::shared_ptr<Box> p;
        if (convert<std::shared_ptr<Box>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == ConstantMedium::NAME) {
        std::shared_ptr<ConstantMedium> p;
        if (convert<std::shared_ptr<ConstantMedium>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == HittableList::NAME) {
        std::shared_ptr<HittableList> p;
        if (convert<std::shared_ptr<HittableList>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    } else if (type == BVHNode::NAME) {
        std::shared_ptr<BVHNode> p;
        if (convert<std::shared_ptr<BVHNode>>::decode(node, p)) {
            rhs = p;
            return true;
        }
    }

    return false;
}

bool convert<std::shared_ptr<Hittable>>::decode(const Node &node, const std::unordered_map<std::string, std::shared_ptr<Hittable>> &refs, const std::unordered_map<std::string, std::shared_ptr<Material>> &materials, const std::unordered_map<std::string, std::shared_ptr<Texture>> &textures, std::shared_ptr<Hittable> &rhs) {
    if (!node.IsMap()) return false;

    const auto type = node["type"].as<std::string>();
    if (type == "sphere") {
        std::shared_ptr<Sphere> p;
        if (convert<std::shared_ptr<Sphere>>::decode(node, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "quad") {
        std::shared_ptr<Quad> p;
        if (convert<std::shared_ptr<Quad>>::decode(node, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "translate") {
        std::shared_ptr<Translate> p;
        if (convert<std::shared_ptr<Translate>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "rotate-y") {
        std::shared_ptr<RotateY> p;
        if (convert<std::shared_ptr<RotateY>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == "box") {
        std::shared_ptr<Box> p;
        if (convert<std::shared_ptr<Box>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == ConstantMedium::NAME) {
        std::shared_ptr<ConstantMedium> p;
        if (convert<std::shared_ptr<ConstantMedium>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == HittableList::NAME) {
        std::shared_ptr<HittableList> p;
        if (convert<std::shared_ptr<HittableList>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    } else if (type == BVHNode::NAME) {
        std::shared_ptr<BVHNode> p;
        if (convert<std::shared_ptr<BVHNode>>::decode(node, refs, materials, textures, p)) {
            rhs = p;
            return true;
        }
    }

    return false;
}
};
