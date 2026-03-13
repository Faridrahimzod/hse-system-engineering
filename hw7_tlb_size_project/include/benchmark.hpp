#pragma once

#include "result.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace tlb {

    std::vector<BenchmarkResult> RunTlbBenchmark(
        std::size_t page_size_bytes = 4096,
        std::size_t max_pages = 4096,
        std::size_t repetitions = 2000);

    void SaveResultsCsv(const std::string& path, const std::vector<BenchmarkResult>& results);

}  // namespace tlb