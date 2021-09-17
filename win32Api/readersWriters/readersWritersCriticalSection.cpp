#include <windows.h>
#include <iostream>

using namespace std;

struct humanData {
    string name;
    int iterations;
};

bool isDoorOpen = true;
int readersInLibrary = 0;

CRITICAL_SECTION printCriticalSection;
CRITICAL_SECTION writeCriticalSection;
CRITICAL_SECTION enterLibCriticalSection;
CRITICAL_SECTION libDoorCriticalSection;

void synchronizedPrint(const string &msg) {
    EnterCriticalSection(&printCriticalSection);
    cout << msg << endl;
    LeaveCriticalSection(&printCriticalSection);
}

void wait(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' going to wait for " + to_string(seconds) + " seconds!");
    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, seconds * 1000);
}

void readBook(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' going to read book for " + to_string(seconds) + " seconds!");
    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, seconds * 1000);
}

void incrementVisitors() {
    EnterCriticalSection(&enterLibCriticalSection);
    readersInLibrary++;
    LeaveCriticalSection(&enterLibCriticalSection);
}

void decrementVisitors() {
    EnterCriticalSection(&enterLibCriticalSection);
    readersInLibrary--;
    LeaveCriticalSection(&enterLibCriticalSection);
}

int getVisitors() {
    EnterCriticalSection(&enterLibCriticalSection);
    int visitors = readersInLibrary;
    LeaveCriticalSection(&enterLibCriticalSection);
    return visitors;
}

void toggleLibDoor() {
    EnterCriticalSection(&libDoorCriticalSection);
    isDoorOpen = !isDoorOpen;
    LeaveCriticalSection(&libDoorCriticalSection);
}

void read(const string &threadName, const int &iterations) {
    for (int i = 0; i < iterations; ++i) {
        while (true) {
            if (isDoorOpen) {
                synchronizedPrint("'" + threadName + "' enters the library!");
                incrementVisitors();
                readBook(threadName, 3);
                decrementVisitors();
                break;
            } else {
                synchronizedPrint("Door is closed for '" + threadName + "'!");
                wait(threadName, 6);
            }
        }
    }

    synchronizedPrint("'" + threadName + "' go home!");
}

DWORD WINAPI readRoutine(LPVOID lpParams) {
    humanData hm = *(humanData *) lpParams;
    read(hm.name, hm.iterations);
    return 0;
}

void writeBook(const string &threadName) {

    WINBOOL result = 1;
    do {
        if (result == 0) {
            synchronizedPrint("'" + threadName + "' cannot write book. Someone already is writing one...");
            wait(threadName, 24);
        }
        result = TryEnterCriticalSection(&writeCriticalSection);
    } while (result == 0);

    toggleLibDoor();

    while (getVisitors() != 0) {
        synchronizedPrint("'" + threadName + "' wait 3 seconds for all readers to leave library...");
        HANDLE currentThread = GetCurrentThread();
        WaitForSingleObject(currentThread, 3000);
    }

    synchronizedPrint("'" + threadName + "' going to write book for 8 seconds!");
    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, 8000);

    synchronizedPrint("'" + threadName + "' has finished writing the book!");

    toggleLibDoor();

    LeaveCriticalSection(&writeCriticalSection);
}

void write(const string &threadName, const int &iterations) {
    for (int i = 0; i < iterations; ++i) {
        writeBook(threadName);
        wait(threadName, 24);
    }
    synchronizedPrint("'" + threadName + "' is tired and go home!");
}

DWORD WINAPI writeRoutine(LPVOID lpParams) {
    humanData hm = *(humanData *) lpParams;
    write(hm.name, hm.iterations);
    return 0;
}

int main() {
    int readersCount;
    int writersCount;
    int readersIterationCount;
    int writersIterationCount;

    cout << "Enter readers count: ";
    cin >> readersCount;

    cout << "Enter readers iterations: ";
    cin >> readersIterationCount;

    cout << "Enter writers count: ";
    cin >> writersCount;

    cout << "Enter writers iteration: ";
    cin >> writersIterationCount;

    int peopleCount = readersCount + writersCount;
    HANDLE hPeople[peopleCount];
    humanData hd[peopleCount];

    InitializeCriticalSection(&printCriticalSection);
    InitializeCriticalSection(&writeCriticalSection);
    InitializeCriticalSection(&enterLibCriticalSection);
    InitializeCriticalSection(&libDoorCriticalSection);

    for (int i = 0; i < readersCount; ++i) {
        string readerName = "Reader " + to_string(i);
        hd[i].name = readerName;
        hd[i].iterations = readersIterationCount;

        hPeople[i] = CreateThread(nullptr, 0, readRoutine, &hd[i], 0, nullptr);
    }

    for (int i = 0; i < writersCount; ++i) {
        string writerName = "Writer " + to_string(i);
        hd[readersCount + i].name = writerName;
        hd[readersCount + i].iterations = writersIterationCount;

        hPeople[readersCount + i] = CreateThread(nullptr, 0, writeRoutine, &hd[readersCount + i], 0, nullptr);
    }

    WaitForMultipleObjects(peopleCount, hPeople, TRUE, INFINITE);

    for (HANDLE human: hPeople) {
        CloseHandle(human);
    }

    DeleteCriticalSection(&libDoorCriticalSection);
    DeleteCriticalSection(&enterLibCriticalSection);
    DeleteCriticalSection(&writeCriticalSection);
    DeleteCriticalSection(&printCriticalSection);
    return 0;
}
