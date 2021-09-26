#include <windows.h>
#include <iostream>
#include <unistd.h>
#include <random>
#include <vector>

#define MIN_TIME 1
#define MAX_TIME 10

using namespace std;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(MIN_TIME, MAX_TIME);

struct humanData {
    string name;
    int id;
    int iterations;
};

bool isWriterInLib = false;
int readersInLibrary = 0;

CRITICAL_SECTION printCriticalSection;
CRITICAL_SECTION libDoorCriticalSection;
CRITICAL_SECTION libraryCriticalSection;
CRITICAL_SECTION writersInLibCriticalSection;
CONDITION_VARIABLE writerWaitConditionalVariable;
vector<CRITICAL_SECTION> readersCriticalSections;

void synchronizedPrint(const string &msg) {
    EnterCriticalSection(&printCriticalSection);
    cout << msg << endl;
    LeaveCriticalSection(&printCriticalSection);
}

void wait(const string &threadName) {
    double seconds = dist(mt);
    synchronizedPrint("'" + threadName + "' going to wait for " + to_string(seconds) + " seconds!");
    sleep(seconds);
}

void readBook(const string &threadName) {
    double seconds = dist(mt);
    synchronizedPrint("'" + threadName + "' going to read book for " + to_string(seconds) + " seconds! " +
                      " Reader in library: " + to_string(readersInLibrary));
    sleep(seconds);
}

void read(const string &threadName, const int &iterations, const int &id) {
    for (int i = 0; i < iterations; ++i) {

        while (isWriterInLib) {
            synchronizedPrint("'" + threadName + "' can not access library, cause writer in lib!");
            SleepConditionVariableCS(&writerWaitConditionalVariable, &readersCriticalSections[i], INFINITE);
        }
        LeaveCriticalSection(&readersCriticalSections[i]);

        EnterCriticalSection(&libDoorCriticalSection);
        if (readersInLibrary == 0) {
            EnterCriticalSection(&libraryCriticalSection);
        }
        readersInLibrary++;
        LeaveCriticalSection(&libDoorCriticalSection);

        readBook(threadName);

        EnterCriticalSection(&libDoorCriticalSection);
        synchronizedPrint(
                "'" + threadName + "' is leaving library, readers in library: " + to_string(readersInLibrary));
        if (readersInLibrary == 1) {
            LeaveCriticalSection(&libraryCriticalSection);
        }
        readersInLibrary--;
        LeaveCriticalSection(&libDoorCriticalSection);
    }

    synchronizedPrint("'" + threadName + "' go home!");
}

DWORD WINAPI readRoutine(LPVOID lpParams) {
    humanData hm = *(humanData *) lpParams;
    read(hm.name, hm.iterations, hm.id);
    return 0;
}

void writeBook(const string &threadName) {
    double seconds = dist(mt);
    synchronizedPrint("'" + threadName + "' going to write book for " + to_string(seconds) + " seconds!");
    sleep(seconds);
}

void write(const string &threadName) {
    EnterCriticalSection(&writersInLibCriticalSection);
    synchronizedPrint("'" + threadName + "' writer enters lib!");
    isWriterInLib = true;

    EnterCriticalSection(&libraryCriticalSection);
    writeBook(threadName);
    LeaveCriticalSection(&libraryCriticalSection);

    isWriterInLib = false;
    WakeAllConditionVariable(&writerWaitConditionalVariable);
    wait(threadName);
    LeaveCriticalSection(&writersInLibCriticalSection);
}

void write(const string &threadName, const int &iterations) {
    for (int i = 0; i < iterations; ++i) {
        write(threadName);
        wait(threadName);
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

    for (int i = 0; i < readersCount; ++i) {
        CRITICAL_SECTION cs;
        InitializeCriticalSection(&cs);
        readersCriticalSections.push_back(cs);
    }

    InitializeCriticalSection(&printCriticalSection);
    InitializeCriticalSection(&libDoorCriticalSection);
    InitializeCriticalSection(&libraryCriticalSection);
    InitializeCriticalSection(&writersInLibCriticalSection);
    InitializeConditionVariable(&writerWaitConditionalVariable);

    for (int i = 0; i < readersCount; ++i) {
        string readerName = "Reader " + to_string(i);
        hd[i].name = readerName;
        hd[i].id = i;
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

    for (CRITICAL_SECTION cs: readersCriticalSections) {
        DeleteCriticalSection(&cs);
    }

    WakeAllConditionVariable(&writerWaitConditionalVariable);
    DeleteCriticalSection(&writersInLibCriticalSection);
    DeleteCriticalSection(&libraryCriticalSection);
    DeleteCriticalSection(&libDoorCriticalSection);
    DeleteCriticalSection(&printCriticalSection);
    return 0;
}
