#pragma once

#include "future.hpp"

#include <exception>
#include <memory>
#include <utility>

namespace stdlike {

    template <typename T>
    class Promise {
    public:
        Promise()
            : state_(std::make_shared<detail::SharedState<T>>()) {
        }

        Promise(const Promise&) = delete;
        Promise& operator=(const Promise&) = delete;

        Promise(Promise&&) noexcept = default;
        Promise& operator=(Promise&&) noexcept = default;

        Future<T> MakeFuture() {
            return Future<T>{state_};
        }

        void SetValue(T value) {
            if (!state_) {
                return;
            }

            {
                std::lock_guard<std::mutex> lock(state_->mutex);
                state_->result = std::move(value);
                state_->ready = true;
            }

            state_->cv.notify_one();
        }

        void SetException(std::exception_ptr ex) {
            if (!state_) {
                return;
            }

            {
                std::lock_guard<std::mutex> lock(state_->mutex);
                state_->result = ex;
                state_->ready = true;
            }

            state_->cv.notify_one();
        }

    private:
        std::shared_ptr<detail::SharedState<T>> state_;
    };

}  // namespace stdlike