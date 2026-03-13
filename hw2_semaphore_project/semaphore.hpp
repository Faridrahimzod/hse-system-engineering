#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>

namespace solutions {

    class Semaphore {
    public:
        explicit Semaphore(size_t initial)
            : permits_(initial) {
        }

        void Acquire() {
            std::unique_lock<std::mutex> lock(m_);
            while (permits_ == 0) {
                cv_.wait(lock);
            }
            --permits_;
        }

        void Release() {
            std::lock_guard<std::mutex> lock(m_);
            ++permits_;
            cv_.notify_one();
        }

    private:
        size_t permits_{ 0 };
        std::mutex m_;
        std::condition_variable cv_;
    };

}