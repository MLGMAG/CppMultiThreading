#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

struct ClientParams {
    string clientName;
};

struct BarberParams {
    string barberName;
    int barberStamina;
};

HANDLE printingSemaphore;
HANDLE chairsSemaphore;
HANDLE barberSemaphore;
HANDLE sleepSemaphore;
vector<bool> chairs;
bool isBarberSleep = false;

void synchronizedPrint(const string &msg) {
    WaitForSingleObject(printingSemaphore, INFINITE);
    cout << msg << endl;
    ReleaseSemaphore(printingSemaphore, 1, nullptr);
}

size_t captureChairIndex() {
    WaitForSingleObject(chairsSemaphore, INFINITE);
    for (size_t i = 0; i < chairs.size(); ++i) {
        if (chairs[i]) {
            ReleaseSemaphore(chairsSemaphore, 1, nullptr);
            return i;
        }
    }
    ReleaseSemaphore(chairsSemaphore, 1, nullptr);
    return -1;
}

void clearChair(const size_t &chairIndex) {
    WaitForSingleObject(chairsSemaphore, INFINITE);
    chairs[chairIndex] = false;
    ReleaseSemaphore(chairsSemaphore, 1, nullptr);
}

void toggleSleep() {
    WaitForSingleObject(sleepSemaphore, INFINITE);
    isBarberSleep = !isBarberSleep;
    ReleaseSemaphore(sleepSemaphore, 1, nullptr);
}

bool isSleep() {
    WaitForSingleObject(sleepSemaphore, INFINITE);
    bool value = isBarberSleep;
    ReleaseSemaphore(sleepSemaphore, 1, nullptr);
    return value;
}

void barbering(const string &barberName, const int &barberStamina) {
    for (int i = 0; i < barberStamina; ++i) {
        synchronizedPrint("'" + barberName + "': There are no clients. I go sleep...");
        toggleSleep();
        WaitForSingleObject(barberSemaphore, INFINITE);
        toggleSleep();

        synchronizedPrint("'" + barberName + "': Hi client!");

        size_t clientChairIndex = captureChairIndex();

        while (clientChairIndex != -1) {
            synchronizedPrint(
                    "'" + barberName + "': I am working with chair " + to_string(clientChairIndex) + " for 3 seconds!");
            HANDLE currentThread = GetCurrentThread();
            WaitForSingleObject(currentThread, 3000);
            synchronizedPrint("'" + barberName + "': I am finished with chair " + to_string(clientChairIndex));
            clearChair(clientChairIndex);
            WaitForSingleObject(currentThread, 1000);
            ReleaseSemaphore(barberSemaphore, 1, nullptr);
            clientChairIndex = captureChairIndex();
        }
        WaitForSingleObject(barberSemaphore, INFINITE);
    }
    synchronizedPrint("'" + barberName + "': I am tired, I go home!");
}

DWORD WINAPI barberingRoutine(LPVOID lpParams) {
    BarberParams bp = *(BarberParams *) lpParams;
    barbering(bp.barberName, bp.barberStamina);
    return 0;
}

size_t takeChair() {
    WaitForSingleObject(chairsSemaphore, INFINITE);
    for (size_t i = 0; i < chairs.size(); ++i) {
        if (!chairs[i]) {
            chairs[i] = true;
            ReleaseSemaphore(chairsSemaphore, 1, nullptr);
            return i;
        }
    }
    ReleaseSemaphore(chairsSemaphore, 1, nullptr);
    return -1;
}

bool getChairValueOnIndex(size_t index) {
    WaitForSingleObject(chairsSemaphore, INFINITE);
    bool value = chairs[index];
    ReleaseSemaphore(chairsSemaphore, 1, nullptr);
    return value;
}

void hairCut(const string &clientName) {
    synchronizedPrint("'" + clientName + "' enters to barbary...");
    size_t chairIndex = takeChair();

    if (chairIndex == -1) {
        synchronizedPrint("No chairs for '" + clientName + "'. He goes walk for 10 seconds");
        HANDLE currentThread = GetCurrentThread();
        WaitForSingleObject(currentThread, 10000);
        hairCut(clientName);
        return;
    }

    synchronizedPrint("'" + clientName + "' takes chair and waits...");
    if (isSleep()) {
        ReleaseSemaphore(barberSemaphore, 1, nullptr);
    }

    while (getChairValueOnIndex(chairIndex)) {
        HANDLE currentThread = GetCurrentThread();
        WaitForSingleObject(currentThread, 4000);
    }

    synchronizedPrint("'" + clientName + "' lefts barbary");
}

DWORD WINAPI hairCutRoutine(LPVOID lpParams) {
    ClientParams clientParams = *(ClientParams *) lpParams;
    hairCut(clientParams.clientName);
    return 0;
}

int main() {
    int clientCount;
    int clientChairsCount;
    int barberStamina;

    cout << "Enter client count: ";
    cin >> clientCount;

    cout << "Enter client chairs count: ";
    cin >> clientChairsCount;

    cout << "Enter barber stamina: ";
    cin >> barberStamina;

    HANDLE threads[clientCount + 1];
    ClientParams cp[clientCount];
    printingSemaphore = CreateSemaphoreA(nullptr, 1, 1, nullptr);
    barberSemaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
    chairsSemaphore = CreateSemaphoreA(nullptr, 1, 1, nullptr);
    sleepSemaphore = CreateSemaphoreA(nullptr, 1, 1, nullptr);
    chairs = vector<bool>(clientChairsCount, false);

    BarberParams bp{"Barber", barberStamina};
    threads[0] = CreateThread(nullptr, 0, barberingRoutine, &bp, 0, nullptr);

    HANDLE currentThread = GetCurrentThread();
    WaitForSingleObject(currentThread, 3000);

    for (int i = 0; i < clientCount; ++i) {
        string clientName = "Client " + to_string(i);
        cp[i].clientName = clientName;

        threads[i + 1] = CreateThread(nullptr, 0, hairCutRoutine, &cp[i], 0, nullptr);
    }

    WaitForMultipleObjects(clientCount + 1, threads, TRUE, INFINITE);

    for (HANDLE thread: threads) {
        CloseHandle(thread);
    }

    CloseHandle(sleepSemaphore);
    CloseHandle(chairsSemaphore);
    CloseHandle(barberSemaphore);
    CloseHandle(printingSemaphore);

    return 0;
}
