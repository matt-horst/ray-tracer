#pragma once

#include "util.hpp"
#include "point3.hpp"

class Perlin {
public:
    Perlin() {
        for (int i = 0; i < point_count; i++) {
            randvec_[i] = Vec3<double>::random(-1, 1);
        }

        perlin_generate_perm(perm_x_);
        perlin_generate_perm(perm_y_);
        perlin_generate_perm(perm_z_);
    }

    double noise(const Point3<double> &p) const {
        auto u = p.x() - std::floor(p.x());
        auto v = p.y() - std::floor(p.y());
        auto w = p.z() - std::floor(p.z());

        const auto i = static_cast<int>(std::floor(p.x()));
        const auto j = static_cast<int>(std::floor(p.y()));
        const auto k = static_cast<int>(std::floor(p.z()));

        Vec3<double> c[2][2][2];

        for (int di = 0; di < 2; di++) {
            for (int dj = 0; dj < 2; dj++) {
                for (int dk = 0; dk < 2; dk++) {
                    c[di][dj][dk] = randvec_[perm_x_[(i + di) & 255] ^ perm_y_[(j + dj) & 255] ^ perm_z_[(k + dk) & 255]];
                }
            }
        }

        return perlin_interp(c, u, v, w);
    }

    double turb(const Point3<double> p, int depth) const {
        auto acc = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            acc += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }

        return std::fabs(acc);
    }

private:
    static const int point_count = 256;
    Vec3<double> randvec_[point_count];
    int perm_x_[point_count];
    int perm_y_[point_count];
    int perm_z_[point_count];

    static void perlin_generate_perm(int *p) {
        for (int i = 0; i < point_count; i++) {
            p[i] = i;
        }

        permute(p, point_count);
    }

    static void permute(int *p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double trilinear_interp(double c[2][2][2], double u, double v, double w) {
        auto acc = 0.0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    acc += (i * u + (1 - i) * (1 - u)) 
                        * (j * v + (1 - j) * (1 - v))
                        * (k * w + (1 - k) * (1 - w))
                        * c[i][j][k];
                }
            }
        }

        return acc;
    }

    static double perlin_interp(Vec3<double> c[2][2][2], double u, double v, double w) {
        const auto uu = u * u * (3 - 2 * u);
        const auto vv = v * v * (3 - 2 * v);
        const auto ww = w * w * (3 - 2 * w);

        auto acc = 0.0;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    Vec3<double> weight_vec(u - i, v - j, w - k);
                    acc += (i * uu + (1 - i) * (1 - uu)) 
                        * (j * vv + (1 - j) * (1 - vv))
                        * (k * ww + (1 - k) * (1 - ww))
                        * dot(c[i][j][k], weight_vec);
                }
            }
        }

        return acc;
    }
};
