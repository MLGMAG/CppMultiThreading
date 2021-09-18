#include <iostream>
#include <windows.h>

#define THREAD_COUNT 10
#define SEMAPHORE_MAX_COUNT 3

using namespace std;

struct ThreadData {
    string threadName;
};

HANDLE semaphore;
HANDLE printMutex;

void synchronizedPrint(const string &msg) {
    WaitForSingleObject(printMutex, INFINITE);
    cout << msg << endl;
    ReleaseMutex(printMutex);
}

DWORD WINAPI ThreadFunction(LPVOID lpParams) {
    ThreadData td = *(ThreadData *) lpParams;
    WaitForSingleObject(semaphore, INFINITE);
    synchronizedPrint("'" + td.threadName + "' works hard...");
    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, 3000);
    ReleaseSemaphore(semaphore, 1, nullptr);
    return 0;
}

int main() {
    HANDLE threads[THREAD_COUNT];
    ThreadData td[THREAD_COUNT];
    semaphore = CreateSemaphoreA(nullptr, SEMAPHORE_MAX_COUNT, SEMAPHORE_MAX_COUNT, nullptr);
    printMutex = CreateMutexA(nullptr, FALSE, nullptr);

    for (int i = 0; i < THREAD_COUNT; ++i) {
        string threadName = "Thread " + to_string(i);
        td[i].threadName = threadName;

        threads[i] = CreateThread(nullptr, 0, ThreadFunction, &td[i], 0, nullptr);
    }

    WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);

    for (HANDLE thread: threads) {
        CloseHandle(thread);
    }

    CloseHandle(printMutex);
    CloseHandle(semaphore);

    return 0;
}
