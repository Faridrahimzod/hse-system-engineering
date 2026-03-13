#pragma once

#include <atomic>
#include <cstdint>

namespace stdlike {

    class CondVar {
    public:
        template <class Mutex>
        void Wait(Mutex& mutex) {
            // Запоминаем текущую версию condvar
            uint32_t old = seq_.load();

            // Отпускаем внешний мьютекс перед сном
            mutex.unlock();

            // Ждем, пока значение seq_ не изменится
            seq_.wait(old);

            // После пробуждения снова захватываем мьютекс
            mutex.lock();
        }

        void NotifyOne() {
            seq_.fetch_add(1);
            seq_.notify_one();
        }

        void NotifyAll() {
            seq_.fetch_add(1);
            seq_.notify_all();
        }

    private:
        std::atomic<uint32_t> seq_{ 0 };
    };

}