#include <pthread.h>
#include <iostream>
#include <string>

using namespace std;

void *threadFun(void *params) {

    string threadName = *(string *) params;
    cout << "Hello, it's " << threadName << "!" << endl;

    return nullptr;
}

int main() {
    pthread_t tid;
    string threadName = "Thread 1";

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, threadFun, &threadName);

    pthread_join(tid, nullptr);

    cout << "End of main thread..." << endl;

    return 0;
}
