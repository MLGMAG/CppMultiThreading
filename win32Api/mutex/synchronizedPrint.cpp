#include <windows.h>
#include <iostream>

#define MAX_THREADS 10
#define ITERATIONS 3

using namespace std;

HANDLE hMutex;

struct ExecuteFunctionParams {
    string threadName;
};

void synchronizedPrint(const string &msg) {
    WaitForSingleObject(hMutex, INFINITE);
    cout << msg << endl;
    ReleaseMutex(hMutex);
}

void sleep(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes sleep for " + to_string(seconds) + " seconds!");
    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, seconds * 1000);
}

void execute(const string &threadName) {
    for (int i = 0; i < ITERATIONS; ++i) {
        string msg = "It's '" + threadName + "'! Iteration: " + to_string(i);
        synchronizedPrint(msg);
        sleep(threadName, 3);
    }
}

DWORD WINAPI ThreadFunction(LPVOID lpParams) {
    ExecuteFunctionParams executeFunctionParams = *(ExecuteFunctionParams *) lpParams;
    execute(executeFunctionParams.threadName);
    return 0;
}

int main() {
    hMutex = CreateMutexA(
            nullptr,
            FALSE,
            nullptr
    );

    HANDLE hThreadArray[MAX_THREADS];
    ExecuteFunctionParams pDataArray[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; ++i) {
        string threadName = "Thread " + to_string(i);

        pDataArray[i].threadName = threadName;

        synchronizedPrint("Create thread '" + threadName + "'");
        hThreadArray[i] = CreateThread(
                nullptr,
                0,
                ThreadFunction,
                &pDataArray[i],
                0,
                nullptr
        );
    }

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);

    synchronizedPrint("Starting to close handles!");
    for (auto &i: hThreadArray) {
        CloseHandle(i);
    }
    CloseHandle(hMutex);

    return 0;
}
