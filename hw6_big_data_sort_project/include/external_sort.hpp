#pragma once

#include "types.hpp"

#include <cstddef>
#include <string>

namespace bigsort {

	std::size_t DefaultMemoryLimitBytes(const std::string& path);

	void SortFile(const std::string& path, std::size_t memory_limit_bytes);

	bool IsFileSorted(const std::string& path);

}  // namespace bigsort