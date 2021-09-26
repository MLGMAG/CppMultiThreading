#pragma once

#include <pthread.h>

class Semaphore {
private:
    pthread_mutex_t mutex;
    pthread_cond_t condition_;
    unsigned long max_count_ = 1;
    unsigned long count_ = max_count_;

public:
    Semaphore();

    explicit Semaphore(unsigned long max_count);

    explicit Semaphore(unsigned long max_count, unsigned long initial_size);

    ~Semaphore();

    unsigned long max();

    unsigned long getCurrentCount();

    void release();

    void acquire();

    bool try_acquire();

private:
    void init();
};
