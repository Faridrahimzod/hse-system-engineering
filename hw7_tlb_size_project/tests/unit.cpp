#include "benchmark.hpp"

#include <catch2/catch.hpp>

#include <filesystem>

namespace fs = std::filesystem;

TEST_CASE("benchmark returns results", "[tlb]") {
    const auto results = tlb::RunTlbBenchmark(4096, 32, 10);

    REQUIRE_FALSE(results.empty());
    REQUIRE(results.front().pages >= 1);

    for (const auto& row : results) {
        REQUIRE(row.pages >= 1);
        REQUIRE(row.time_ns_per_access >= 0.0);
    }
}

TEST_CASE("save results csv", "[tlb]") {
    const std::string path = "test_results.csv";

    const auto results = tlb::RunTlbBenchmark(4096, 8, 5);
    tlb::SaveResultsCsv(path, results);

    REQUIRE(fs::exists(path));
    REQUIRE(fs::file_size(path) > 0);

    fs::remove(path);
}