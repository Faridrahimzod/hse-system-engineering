#pragma once

#include "shared_state.hpp"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

namespace stdlike {

    template <typename T>
    class Promise;

    template <typename T>
    class Future {
        template <typename U>
        friend class Promise;

    public:
        Future(const Future&) = delete;
        Future& operator=(const Future&) = delete;

        Future(Future&&) noexcept = default;
        Future& operator=(Future&&) noexcept = default;

        T Get() {
            if (!state_) {
                return T{};
            }

            std::unique_lock<std::mutex> lock(state_->mutex);
            while (!state_->ready) {
                state_->cv.wait(lock);
            }

            if (std::holds_alternative<std::exception_ptr>(state_->result)) {
                std::exception_ptr ex = std::get<std::exception_ptr>(state_->result);
                lock.unlock();
                state_.reset();
                std::rethrow_exception(ex);
            }

            T value = std::move(std::get<T>(state_->result));
            lock.unlock();
            state_.reset();
            return value;
        }

    private:
        Future() = default;

        explicit Future(std::shared_ptr<detail::SharedState<T>> state)
            : state_(std::move(state)) {
        }

    private:
        std::shared_ptr<detail::SharedState<T>> state_;
    };

}  // namespace stdlike