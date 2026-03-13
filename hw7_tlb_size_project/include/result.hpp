#pragma once

#include <cstddef>

namespace tlb {

	struct BenchmarkResult {
		std::size_t pages{ 0 };
		double time_ns_per_access{ 0.0 };
	};

}  // namespace tlb