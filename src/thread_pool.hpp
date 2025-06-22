#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <optional>

class TaskGenerator {
public:
    virtual std::optional<std::function<void()>> next() = 0;
    virtual bool has_next() const = 0;
    virtual double progress(size_t count) const = 0;
    virtual ~TaskGenerator() = default;
};

class ThreadPool {
public:
    ThreadPool(TaskGenerator& generator,  size_t num_threads = std::thread::hardware_concurrency()) : num_threads(num_threads), generator(generator) {
        // Spawn worker threads
        for (size_t i = 0; i < num_threads; i++) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    { 
                        std::unique_lock<std::mutex> lock(task_mutex);

                        std::optional<std::function<void()>> task_opt = this->generator.next();
                        if (task_opt.has_value()) {
                            task = task_opt.value();
                        } else {
                            return;
                        }
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
        for (auto& worker : workers) {
            worker.join();
        }
    }

    size_t count() {
        std::unique_lock<std::mutex> lock(counter_mutex);
        return count_;
    }

    bool has_next() {
        return generator.has_next();
    }

    void kill() {
        for (auto& worker : workers) {
            worker.join();
        }

        workers.clear();
    }

    double progress() { 
        std::unique_lock lock(counter_mutex);
        return generator.progress(count_); 
    }

    size_t num_threads;


private:
    std::vector<std::thread> workers;
    std::mutex task_mutex;
    TaskGenerator& generator;
    std::mutex counter_mutex;
    size_t count_ = 0;
};
