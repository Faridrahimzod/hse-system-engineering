#pragma once

#include "tagged_semaphore.hpp"

#include <cstddef>
#include <deque>
#include <utility>

namespace solutions {

    template <typename T>
    class BlockingQueue {
    private:
        struct QueueTag {};

    public:
        explicit BlockingQueue(size_t capacity)
            : slots_(capacity),
            items_(0),
            mutex_(1) {
        }

        void Put(T value) {
            auto slot = slots_.Acquire();
            auto guard = mutex_.Acquire();

            buffer_.push_back(std::move(value));

            mutex_.Release(std::move(guard));
            items_.Release(std::move(slot));
        }
        T Take() {
            auto item = items_.Acquire();
            auto guard = mutex_.Acquire();

            T value = std::move(buffer_.front());
            buffer_.pop_front();

            mutex_.Release(std::move(guard));
            slots_.Release(std::move(item));

            return value;
        }

    private:
        std::deque<T> buffer_;
        TaggedSemaphore<QueueTag> slots_;
        TaggedSemaphore<QueueTag> items_;
        TaggedSemaphore<QueueTag> mutex_;
    };

}