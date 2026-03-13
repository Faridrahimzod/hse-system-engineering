#pragma once

#include <condition_variable>
#include <exception>
#include <mutex>
#include <variant>

namespace stdlike::detail {

	template <typename T>
	struct SharedState {
		std::mutex mutex;
		std::condition_variable cv;
		bool ready{ false };
		std::variant<T, std::exception_ptr> result;
	};

}  // namespace stdlike::detail