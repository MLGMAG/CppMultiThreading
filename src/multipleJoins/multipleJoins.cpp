#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <mutex>

using namespace std;

mutex mtx;

void synchronizedPrint(const string &msg) {
    lock_guard guard(mtx);
    cout << msg << endl;
}

void sleep(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' go sleep for " + to_string(seconds) + " seconds");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void printThreadData(const string &threadName, const int &sleepTime) {
    synchronizedPrint("Hi! It's '" + threadName + "'");
    sleep(threadName, sleepTime);
    synchronizedPrint("'" + threadName + "' is completed!");
}

int main() {
    vector<string> threadsNames = {"Thread 1", "Thread 2", "Thread 3", "Thread 4"};
    vector<thread> threads;

    threads.reserve(threadsNames.size());
    for (size_t i = 0; i < threadsNames.size(); i++) {
        threads.emplace_back(printThreadData, threadsNames[i], i + 1);
    }

    for (thread &t: threads) {
        t.join();
    }

    synchronizedPrint("Main thread is completed!");
}
