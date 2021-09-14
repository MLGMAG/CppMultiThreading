#include <pthread.h>
#include <iostream>
#include <string>

#define THREAD_COUNT 10

using namespace std;

struct threadData {
    string threadName;
};

pthread_mutex_t mutex;

void synchronizedPrint(const string &msg) {
    pthread_mutex_lock(&mutex);
    cout << msg << endl;
    pthread_mutex_unlock(&mutex);
}

void *print(void *params) {
    threadData td = *(threadData *) params;

    for (int i = 0; i < 5; ++i) {
        synchronizedPrint(td.threadName + " iteration: " + to_string(i));
    }

    pthread_exit(nullptr);
}

int main() {
    pthread_t threads[THREAD_COUNT];
    struct threadData td[THREAD_COUNT];
    pthread_mutex_init(&mutex, nullptr);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        string threadName = "Thread " + to_string(i);
        td[i].threadName = threadName;

        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_create(&threads[i], &attr, print, &td[i]);
    }

    for (pthread_t thread: threads) {
        pthread_join(thread, nullptr);
    }

    pthread_mutex_destroy(&mutex);
    pthread_exit(nullptr);
}
