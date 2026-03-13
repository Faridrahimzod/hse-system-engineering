#pragma once

#include <atomic>
#include <cstdint>

namespace stdlike {

    class Mutex {
    public:
        void Lock() {
            uint32_t expected = kUnlocked;
            if (state_.compare_exchange_strong(expected, kLockedNoWaiters,
                std::memory_order_acquire,
                std::memory_order_relaxed)) {
                return;
            }

            while (true) {
                if (expected == kUnlocked) {
                    if (state_.compare_exchange_strong(expected, kLockedWithWaiters,
                        std::memory_order_acquire,
                        std::memory_order_relaxed)) {
                        return;
                    }
                    continue;
                }

                if (expected == kLockedNoWaiters) {
                    if (!state_.compare_exchange_strong(expected, kLockedWithWaiters,
                        std::memory_order_relaxed,
                        std::memory_order_relaxed)) {
                        continue;
                    }
                }

                state_.wait(kLockedWithWaiters, std::memory_order_relaxed);
                expected = kUnlocked;
            }
        }

        void Unlock() {
            uint32_t old = state_.exchange(kUnlocked, std::memory_order_release);
            if (old == kLockedWithWaiters) {
                state_.notify_one();
            }
        }

    private:
        static constexpr uint32_t kUnlocked = 0;
        static constexpr uint32_t kLockedNoWaiters = 1;
        static constexpr uint32_t kLockedWithWaiters = 2;

    private:
        std::atomic<uint32_t> state_{ kUnlocked };
    };

}  // namespace stdlike