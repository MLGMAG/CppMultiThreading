#pragma once

#include <mutex>
#include <condition_variable>

class Semaphore {
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    unsigned long max_count_ = 1;
    unsigned long count_ = max_count_;

public:
    Semaphore();

    explicit Semaphore(unsigned long max_count);

    unsigned long max();

    unsigned long getCurrentCount();

    void release();

    void acquire();

    bool try_acquire();
};