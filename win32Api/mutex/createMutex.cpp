#include <windows.h>
#include <iostream>

using namespace std;

int main() {
    HANDLE hMutex;

    hMutex = CreateMutexA(
            nullptr,
            FALSE,
            nullptr
    );

    if (hMutex == nullptr) {
        cout << "Failed to create mutex! Error: " << GetLastError() << endl;
    } else {
        cout << "Mutex created successfully!" << endl;
    }

    CloseHandle(hMutex);

    return 0;
}
