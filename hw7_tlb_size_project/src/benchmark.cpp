#include "benchmark.hpp"

#include "timer.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace tlb {
    namespace {

        std::vector<std::size_t> MakePageCounts(std::size_t max_pages) {
            std::vector<std::size_t> counts;
            for (std::size_t p = 1; p <= 16 && p <= max_pages; ++p) {
                counts.push_back(p);
            }

            for (std::size_t p = 24; p <= max_pages; p += 8) {
                counts.push_back(p);
            }

            counts.erase(std::unique(counts.begin(), counts.end()), counts.end());
            return counts;
        }

        double MeasurePages(std::size_t pages, std::size_t page_size_bytes, std::size_t repetitions) {
            const std::size_t ints_per_page = page_size_bytes / sizeof(std::uint32_t);
            const std::size_t total_ints = pages * ints_per_page;

            std::vector<std::uint32_t> data(total_ints, 1);

            volatile std::uint64_t sink = 0;

            for (std::size_t i = 0; i < pages; ++i) {
                sink += data[i * ints_per_page];
            }

            SteadyTimer timer;
            timer.Start();

            for (std::size_t rep = 0; rep < repetitions; ++rep) {
                for (std::size_t i = 0; i < pages; ++i) {
                    sink += data[i * ints_per_page];
                }
            }

            const std::uint64_t elapsed_ns = timer.StopNs();

            if (sink == 0xFFFFFFFFFFFFFFFFull) {
                throw std::runtime_error("Impossible sink value");
            }

            const double accesses = static_cast<double>(pages) * static_cast<double>(repetitions);
            return static_cast<double>(elapsed_ns) / accesses;
        }

    }  // namespace

    std::vector<BenchmarkResult> RunTlbBenchmark(
        std::size_t page_size_bytes,
        std::size_t max_pages,
        std::size_t repetitions) {
        if (page_size_bytes < sizeof(std::uint32_t)) {
            throw std::runtime_error("Page size is too small");
        }
        if (max_pages == 0 || repetitions == 0) {
            throw std::runtime_error("Invalid benchmark parameters");
        }

        std::vector<BenchmarkResult> results;
        const auto page_counts = MakePageCounts(max_pages);
        results.reserve(page_counts.size());

        for (std::size_t pages : page_counts) {
            const double ns_per_access = MeasurePages(pages, page_size_bytes, repetitions);
            results.push_back(BenchmarkResult{ pages, ns_per_access });
        }

        return results;
    }

    void SaveResultsCsv(const std::string& path, const std::vector<BenchmarkResult>& results) {
        std::ofstream out(path, std::ios::trunc);
        if (!out) {
            throw std::runtime_error("Failed to open output CSV");
        }

        out << "pages,time_ns_per_access\n";
        for (const auto& row : results) {
            out << row.pages << ',' << row.time_ns_per_access << '\n';
        }

        if (!out) {
            throw std::runtime_error("Failed to write CSV");
        }
    }

}  // namespace tlb