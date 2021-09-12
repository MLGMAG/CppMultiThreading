#include <iostream>
#include "semaphore.h"
#include <thread>
#include <vector>

#define SEMAPHORE_MAX_COUNT 2
#define THREADS_COUNT 6

using namespace std;

mutex mtx;
Semaphore semaphore(SEMAPHORE_MAX_COUNT);

void synchronizedPrint(const string &msg) {
    lock_guard guard(mtx);
    cout << msg << endl;
}

int main() {
    auto hardWork = [](const string &threadName) {
        semaphore.acquire();
        synchronizedPrint("Thread '" + threadName + "' works hard for 5 seconds!");

        this_thread::sleep_for(chrono::seconds(5));

        semaphore.release();
        synchronizedPrint("Thread '" + threadName + "' complete work!");
    };

    vector<thread> threads;
    int threadsCount = THREADS_COUNT;

    threads.reserve(threadsCount);
    for (int i = 0; i < threadsCount; ++i) {
        threads.emplace_back(hardWork, to_string(i));
    }

    for (int i = 0; i < threadsCount; ++i) {
        threads[i].join();
    }
}
