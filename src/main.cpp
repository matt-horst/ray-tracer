#include <chrono>
#include <condition_variable>
#include <format>
#include <iostream>
#include <memory>
#include <queue>
#include <unistd.h>
#include <thread>
#include <functional>
#include <mutex>
#include <cassert>
#include "camera.hpp"
#include "sphere.hpp"
#include "hittable_list.hpp"
#include "material.hpp"

class ThreadPool {
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency()) {
        // Spawn worker threads
        for (size_t i = 0; i < num_threads; i++) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    { std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [this] {return !tasks.empty() || stop;});

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(counter_mutex);
                        count_++;
                    }
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }

        cv.notify_all();

        for (auto& worker : workers) {
            worker.join();
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::move(task));
        }

        cv.notify_one();
    }

    size_t count() {
        std::unique_lock<std::mutex> lock(counter_mutex);
        return count_;
    }

    void kill() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }

        cv.notify_all();

        for (auto& worker : workers) {
            worker.join();
        }

        workers.clear();
    }


private:
    std::vector<std::thread> workers;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::queue<std::function<void()>> tasks;
    bool stop = false;
    std::mutex counter_mutex;
    size_t count_ = 0;
};

class ImageChunk {
public:
    int32_t x, y, width, height;
    std::vector<std::vector<Color>> &pixels;

    ImageChunk(int32_t x, int32_t y, int32_t w, int32_t h, std::vector<std::vector<Color>> &pixels) : x(x), y(y), width(w), height(h), pixels(pixels) {}
};

class Image {
public:
    int32_t width, height;
    int32_t num_chunks;

    ImageChunk get(size_t id) {
        return ImageChunk(id / chunks_per_row * chunk_height, (id % chunks_per_row) * chunk_width, chunk_width, chunk_height, pixels);
    }

    void write() const {
        std::cout << "P3\n";
        std::cout << width << ' ' << height << '\n';
        std::cout << "255\n";

        for (int32_t i = 0; i < height; i++) {
            for (int32_t j = 0; j < width; j++) {
                const Color &color = pixels[i][j];
                std::cout << color << '\n';
            }
        }
    }

    Image(int32_t width, int32_t height, int32_t chunk_width, int32_t chunk_height) :
        width(width),
        height(height),
        num_chunks(width * height  / (chunk_width * chunk_height)),
        chunk_width(chunk_width),
        chunk_height(chunk_height),
        chunks_per_row(width / chunk_width),
        pixels(std::vector<std::vector<Color>>(height, std::vector<Color>(width)))
    {
        assert(width % chunk_width == 0 && "Chunk width must be a factor of total width.");
        assert(height % chunk_height == 0 &&  "Chunk height must be a factor of total height.");
    }

private:
    int32_t chunk_width, chunk_height;
    int32_t chunks_per_row;
    std::vector<std::vector<Color>> pixels;
};

Color ray_color(const Ray<double> &ray, const Hittable &world, int32_t depth, int32_t max_depth) {
    if (depth >= max_depth) return Color();

    HitRecord rec;
    if (world.hit(ray, Interval(0.001, infinity), rec)) {
        Ray<double> scattered;
        Color attenuation;
        if (rec.mat->scatter(ray, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth + 1, max_depth);
        }

        return Color();
    }

    const Vec3<double> unit_direction = normalize(ray.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

void render_chunk(const Camera& cam, const HittableList& scene, ImageChunk img) {
    for (int32_t i = img.x; i < img.x + img.height; i++) {
        for (int32_t j = img.y; j < img.y + img.width; j++) {
            Color pixel_color(0.0, 0.0, 0.0);
            for (int32_t k = 0; k < cam.samples_per_pixel; k++) {
                Ray<double> ray = cam.cast_ray_at_pixel_loc(i, j);
                pixel_color += ray_color(ray, scene, 0, cam.max_depth);
            }

            img.pixels[i][j] = pixel_color * cam.pixel_color_scale;
        }
    }
}

void render(const Camera& cam, const HittableList& scene, int num_threads) {
    Image img(cam.image_width, cam.image_height, 5, 5);
    ThreadPool pool(num_threads);

    for (int32_t i = 0; i < img.num_chunks; i++) {
        ImageChunk chunk = img.get(i);
        pool.enqueue([&cam, &scene, chunk = std::move(chunk)] { render_chunk(cam, scene, chunk); });
    }

    // Wait
    int32_t count = 0;
    while ((count = pool.count()) < img.num_chunks) {
        const float percentage = (100.0 * count) / img.num_chunks;
        std::clog << std::format("\r{:.2f}% ", percentage) << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Write image to file
    img.write();
}

int main(void) {
    CameraParams params;
    params.image_width = 400;
    params.samples_per_pixel = 100;
    params.max_depth = 50;
    params.lookfrom = Point3<double>(13, 2, 3);
    params.lookat = Point3<double>(0, 1, 0);
    params.vfov = 20.0;
    params.defocus_angle = 0.6;
    params.focus_dist = 10.0;
    const Camera cam(params);

    const auto material_ground = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    const auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    const auto material_left = std::make_shared<Dielectric>(1.5);
    const auto material_bubble = std::make_shared<Dielectric>(1.0 / 1.5);
    const auto material_right = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    HittableList world;
    world.add(std::make_shared<Sphere>(Point3<double>(0.0, -1000.0, 0.0), 1000, material_ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const auto choose_mat = random_double();
            const Point3<double> center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point3<double>(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // Diffuse
                    const auto albedo = Color::random() * Color::random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                } else if (choose_mat < 0.95) {
                    // Metal
                    const auto albedo = Color::random(0.5, 1);
                    const auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                } else {
                    // Glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                }

                world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
            }
        }
    }

    const auto material_1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3<double>(0, 1, 0), 1.0, material_1));

    const auto material_2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3<double>(-4, 1, 0), 1.0, material_2));

    const auto material_3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3<double>(4, 1, 0), 1.0, material_3));

    render(cam, world, 4);
}
