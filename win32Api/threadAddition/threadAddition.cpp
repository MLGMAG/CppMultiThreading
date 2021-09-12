#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;

DWORD WINAPI ThreadFunction(LPVOID lpParams) {
    vector<int> nums = *(vector<int> *) lpParams;
    int num1 = nums[0];
    int num2 = nums[1];
    int result = num1 + num2;

    cout << "Addition result is " << result << endl;
    return 0;
}

int main() {
    HANDLE hThread;
    DWORD ThreadId;

    vector<int> numsToAdd = {1, 2};

    hThread = CreateThread(
            nullptr,
            0,
            ThreadFunction,
            &numsToAdd,
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
