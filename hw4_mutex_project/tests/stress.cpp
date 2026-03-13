#include "mutex.hpp"

#include <catch2/catch.hpp>

#include <atomic>
#include <thread>
#include <vector>

TEST_CASE("mutex stress", "[mutex,stress]") {
    static const int kThreads = 8;
    static const int kIterations = 20000;

    stdlike::Mutex mutex;
    int counter = 0;

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < kIterations; ++i) {
                mutex.Lock();
                ++counter;
                mutex.Unlock();
            }
            });
    }

    for (auto& th : threads) {
        th.join();
    }

    REQUIRE(counter == kThreads * kIterations);
}