#pragma once

#include "bbox.hpp"
#include "hittable.hpp"

#include <memory>
#include <vector>

class HittableList : public Hittable {
public:
    std::vector<std::shared_ptr<Hittable>> objs;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> obj) {add(obj);}

    void clear() { objs.clear(); }

    void add(std::shared_ptr<Hittable> obj) {
        objs.push_back(obj); 
        bbox = BBox3(bbox, obj->bounding_box());
    }

    bool hit(const Ray<double>& ray, Interval ray_t, HitRecord& rec) const override {
        HitRecord temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto &obj : objs) {
            if (obj->hit(ray, Interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    BBox3 bounding_box() const override { return bbox; }

    inline static const std::string NAME = "hittable-list";
private:
    BBox3 bbox;
};
