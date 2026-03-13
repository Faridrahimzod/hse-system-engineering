#include "timer.hpp"

#include <chrono>

namespace tlb {

    std::uint64_t NowNs() {
        const auto now = std::chrono::steady_clock::now().time_since_epoch();
        return static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(now).count());
    }

    void SteadyTimer::Start() {
        start_ns_ = NowNs();
    }

    std::uint64_t SteadyTimer::StopNs() const {
        return NowNs() - start_ns_;
    }

}  // namespace tlb