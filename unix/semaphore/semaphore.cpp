#include <mutex>
#include "semaphore.h"

Semaphore::Semaphore() = default;

Semaphore::Semaphore(unsigned long max_count) : max_count_(max_count) {}

unsigned long Semaphore::max() {
    return max_count_;
}

unsigned long Semaphore::getCurrentCount() {
    return count_;
}

void Semaphore::release() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if (count_ == max_count_) {
        return;
    }

    ++count_;
    condition_.notify_one();
}

void Semaphore::acquire() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    while (!count_) {
        condition_.wait(lock);
    }

    --count_;
}

bool Semaphore::try_acquire() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    if (count_) {
        --count_;
        return true;
    }

    return false;
}
