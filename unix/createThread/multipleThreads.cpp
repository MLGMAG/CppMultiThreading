#include <pthread.h>
#include <iostream>
#include <string>

#define MAX_THREAD_COUNT 10

using namespace std;

struct threadData {
    string threadName;
};

void *threadFun(void *params) {
    string threadName = *(string *) params;
    cout << "Hi, it's " + threadName + "!" << endl;
    pthread_exit(nullptr);
}

int main() {
    pthread_t threads[MAX_THREAD_COUNT];
    struct threadData td[MAX_THREAD_COUNT];

    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        string threadName = "Thread " + to_string(i);
        td[i].threadName = threadName;

        pthread_attr_t attr;
        pthread_attr_init(&attr);

        int rc = pthread_create(&threads[i], &attr, threadFun, &td[i]);

        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }

    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        void *status;
        pthread_join(threads[i], &status);
        cout << td[i].threadName << " exiting with status : " << status << endl;
    }

    cout << "End of main thread..." << endl;
    pthread_exit(nullptr);
}