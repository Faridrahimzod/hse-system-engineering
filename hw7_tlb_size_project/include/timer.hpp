#pragma once

#include <cstdint>

namespace tlb {

	class SteadyTimer {
	public:
		void Start();
		std::uint64_t StopNs() const;

	private:
		std::uint64_t start_ns_{ 0 };
	};

	std::uint64_t NowNs();

}  // namespace tlb