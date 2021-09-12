#include <windows.h>
#include <iostream>

using namespace std;

DWORD WINAPI ThreadFunction(LPVOID lpParams) {
    cout << "Hi, I am child thread!" << endl;
    return 0;
}

int main() {
    HANDLE hThread;
    DWORD ThreadId;

    hThread = CreateThread(
            nullptr,
            0,
            ThreadFunction,
            nullptr,
            0,
            &ThreadId);

    if (hThread == nullptr) {
        cout << "Thread creation is failed! Error: " << GetLastError() << endl;
    } else {
        cout << "Thread created successfully!" << endl;
        cout << "Thread id: " << ThreadId << endl;
    }

    CloseHandle(hThread);

    return 0;
}
