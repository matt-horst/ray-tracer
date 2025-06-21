#pragma once

#include <algorithm>
#include <memory>

#include "bbox.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "interval.hpp"

class BVHNode : public Hittable {
public:
    BVHNode(HittableList list) : BVHNode(list.objs, 0, list.objs.size()) {}

    BVHNode(std::vector<std::shared_ptr<Hittable>> &objs, size_t start, size_t end)  {
        bbox = BBox3::empty;

        for (size_t obj_idx = start; obj_idx < end; obj_idx++) {
            bbox = BBox3(bbox, objs[obj_idx]->bounding_box());
        }

        int axis = bbox.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

        size_t span = end - start;

        if (span == 1) {
            left = right = objs[start];
        } else if (span == 2) {
            left = objs[start];
            right = objs[start + 1];
        } else {
            std::sort(std::begin(objs) + start, std::begin(objs) + end, comparator);

            auto mid = start + span / 2;
            left = std::make_shared<BVHNode>(objs, start, mid);
            right = std::make_shared<BVHNode>(objs, mid, end);
        }
    }

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord &rec) const override {
        if (!bbox.hit(ray, ray_t)) return false;

        bool hit_left = left->hit(ray, ray_t, rec);
        ray_t = Interval(ray_t.min, hit_left ? rec.t : ray_t.max);
        bool hit_right = right->hit(ray, ray_t, rec);

        return hit_left || hit_right;
    }

    BBox3 bounding_box() const override { return bbox; }

    static bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index) {
        const auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        const auto b_axis_interval = b->bounding_box().axis_interval(axis_index);

        return a_axis_interval.min < b_axis_interval.min;
    }

    static bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    BBox3 bbox;
};
