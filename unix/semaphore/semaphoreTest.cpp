#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "semaphore.h"

#define SEMAPHORE_MAX_COUNT 2
#define THREADS_COUNT 6

using namespace std;

pthread_mutex_t printMutex;
Semaphore semaphore(SEMAPHORE_MAX_COUNT);

struct hardWorkParams {
    string threadName;
};

void synchronizedPrint(const string &msg) {
    pthread_mutex_lock(&printMutex);
    cout << msg << endl;
    pthread_mutex_unlock(&printMutex);
}

void hardWork(const string &threadName) {
    semaphore.acquire();
    synchronizedPrint("Thread '" + threadName + "' works hard for 5 seconds!");

    sleep(5);

    semaphore.release();
    synchronizedPrint("Thread '" + threadName + "' complete work!");
}

void *hardWorkRoutine(void *params) {
    hardWorkParams hwp = *(hardWorkParams *) params;

    hardWork(hwp.threadName);

    pthread_exit(nullptr);
}

int main() {
    pthread_t threads[THREADS_COUNT];
    hardWorkParams hwp[THREADS_COUNT];
    pthread_mutex_init(&printMutex, nullptr);

    for (int i = 0; i < THREADS_COUNT; ++i) {
        string threadName = "Thread " + to_string(i);
        hwp[i].threadName = threadName;

        pthread_attr_t pthreadAttr;
        pthread_attr_init(&pthreadAttr);

        pthread_create(&threads[i], &pthreadAttr, &hardWorkRoutine, &hwp[i]);
    }

    for (pthread_t thread: threads) {
        pthread_join(thread, nullptr);
    }

    pthread_mutex_destroy(&printMutex);
    pthread_exit(nullptr);
}
