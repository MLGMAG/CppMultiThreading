#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

HANDLE hMutex;

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

DWORD WINAPI ThreadFunction(LPVOID lpParams) {
    vector<int> nums = *(vector<int> *) lpParams;
    int num1 = nums[0];
    int num2 = nums[1];
    int result = num1 + num2;

    synchronizedPrint("Addition result is " + to_string(result));
    sleep("Calc", 5);
    return 0;
}

int main() {
    HANDLE hThread;
    DWORD ThreadId;

    hMutex = CreateMutexA(
            nullptr,
            FALSE,
            nullptr
    );

    vector<int> numsToAdd = {1, 2};
    hThread = CreateThread(
            nullptr,
            0,
            ThreadFunction,
            &numsToAdd,
            0,
            &ThreadId);

    if (hThread == nullptr) {
        synchronizedPrint("Thread creation is failed! Error: " + to_string(GetLastError()));
    } else {
        synchronizedPrint("Thread created successfully!");
        synchronizedPrint("Thread id: " + to_string(ThreadId));
    }

    WaitForSingleObject(hThread, INFINITE);

    synchronizedPrint("Main thread starts to close Handles!");

    CloseHandle(hThread);
    CloseHandle(hMutex);

    return 0;
}
