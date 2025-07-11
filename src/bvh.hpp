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
        bbox_ = BBox3::empty;

        for (size_t obj_idx = start; obj_idx < end; obj_idx++) {
            bbox_ = BBox3(bbox_, objs[obj_idx]->bounding_box());
        }

        int axis = bbox_.longest_axis();

        auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

        size_t span = end - start;

        if (span == 1) {
            left_ = right_ = objs[start];
        } else if (span == 2) {
            left_ = objs[start];
            right_ = objs[start + 1];
        } else {
            std::sort(std::begin(objs) + start, std::begin(objs) + end, comparator);

            auto mid = start + span / 2;
            left_ = std::make_shared<BVHNode>(objs, start, mid);
            right_ = std::make_shared<BVHNode>(objs, mid, end);
        }
    }

    BVHNode(std::shared_ptr<Hittable> left, std::shared_ptr<Hittable> right) : left_(left), right_(right), bbox_(BBox3(left->bounding_box(), right->bounding_box())) {}

    bool hit(const Ray<double> &ray, Interval ray_t, HitRecord &rec) const override {
        if (!bbox_.hit(ray, ray_t)) return false;

        bool hit_left = left_->hit(ray, ray_t, rec);
        ray_t = Interval(ray_t.min, hit_left ? rec.t : ray_t.max);
        bool hit_right = right_->hit(ray, ray_t, rec);

        return hit_left || hit_right;
    }

    BBox3 bounding_box() const override { return bbox_; }

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

    inline static const std::string NAME = "bvh";

private:
    std::shared_ptr<Hittable> left_;
    std::shared_ptr<Hittable> right_;
    BBox3 bbox_;

    friend struct YAML::convert<std::shared_ptr<BVHNode>>;
};
