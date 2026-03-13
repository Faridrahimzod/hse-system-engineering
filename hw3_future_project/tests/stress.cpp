#include "promise.hpp"

#include <catch2/catch.hpp>

#include <atomic>
#include <thread>
#include <vector>

TEST_CASE("Future stress", "[future,stress]") {
    static const int kThreads = 8;
    static const int kIterations = 1000;

    std::atomic<int> sum{ 0 };
    std::vector<std::thread> threads;

    for (int t = 0; t < kThreads; ++t) {
        threads.emplace_back([&]() {
            for (int i = 0; i < kIterations; ++i) {
                stdlike::Promise<int> p;
                auto f = p.MakeFuture();

                std::thread producer([p = std::move(p)]() mutable {
                    p.SetValue(1);
                    });

                sum.fetch_add(f.Get());
                producer.join();
            }
            });
    }

    for (auto& th : threads) {
        th.join();
    }

    REQUIRE(sum.load() == kThreads * kIterations);
}