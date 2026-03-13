#include "external_sort.hpp"

#include <catch2/catch.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace {

    void WriteValues(const std::string& path, const std::vector<bigsort::Value>& values) {
        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        REQUIRE(static_cast<bool>(out));
        out.write(reinterpret_cast<const char*>(values.data()),
            static_cast<std::streamsize>(values.size() * sizeof(bigsort::Value)));
        REQUIRE(static_cast<bool>(out));
    }

    std::vector<bigsort::Value> ReadValues(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        REQUIRE(static_cast<bool>(in));

        in.seekg(0, std::ios::end);
        const auto size = static_cast<std::size_t>(in.tellg());
        in.seekg(0, std::ios::beg);

        std::vector<bigsort::Value> values(size / sizeof(bigsort::Value));
        in.read(reinterpret_cast<char*>(values.data()),
            static_cast<std::streamsize>(size));
        REQUIRE(static_cast<bool>(in) || in.eof());
        return values;
    }

}  // namespace

TEST_CASE("external sort small file", "[sort]") {
    const std::string path = "test_small.bin";

    WriteValues(path, { 5, 1, 7, -3, 2, 2, 100, 0 });
    bigsort::SortFile(path, 3 * sizeof(bigsort::Value));

    const auto values = ReadValues(path);
    REQUIRE(values == std::vector<bigsort::Value>{-3, 0, 1, 2, 2, 5, 7, 100});
    REQUIRE(bigsort::IsFileSorted(path));

    fs::remove(path);
}

TEST_CASE("external sort empty file", "[sort]") {
    const std::string path = "test_empty.bin";

    WriteValues(path, {});
    bigsort::SortFile(path, sizeof(bigsort::Value));
    REQUIRE(bigsort::IsFileSorted(path));

    fs::remove(path);
}

TEST_CASE("default memory limit", "[sort]") {
    const std::string path = "test_limit.bin";

    WriteValues(path, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
    const auto limit = bigsort::DefaultMemoryLimitBytes(path);
    REQUIRE(limit >= sizeof(bigsort::Value));

    fs::remove(path);
}