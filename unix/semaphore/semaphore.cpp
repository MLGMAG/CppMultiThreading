#include "semaphore.h"

Semaphore::Semaphore() {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition_, nullptr);
};

Semaphore::Semaphore(unsigned long max_count) : max_count_(max_count) {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition_, nullptr);
}

unsigned long Semaphore::max() {
    return max_count_;
}

unsigned long Semaphore::getCurrentCount() {
    return count_;
}

void Semaphore::release() {
    pthread_mutex_lock(&mutex);

    if (count_ == max_count_) {
        return;
    }

    ++count_;
    pthread_cond_signal(&condition_);
    pthread_mutex_unlock(&mutex);
}

void Semaphore::acquire() {
    pthread_mutex_lock(&mutex);

    while (!count_) {
        pthread_cond_wait(&condition_,&mutex);
    }

    --count_;
    pthread_mutex_unlock(&mutex);
}

bool Semaphore::try_acquire() {
    pthread_mutex_lock(&mutex);

    if (count_) {
        --count_;
        pthread_mutex_unlock(&mutex);
        return true;
    }

    pthread_mutex_unlock(&mutex);
    return false;
}

Semaphore::~Semaphore() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition_);
}
