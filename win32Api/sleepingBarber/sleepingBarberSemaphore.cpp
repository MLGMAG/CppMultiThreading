#include <iostream>
#include <windows.h>
#include <vector>
#include "linkedQueue.h"
#include <random>
#include <unistd.h>

#define MIN_TIME 1
#define MAX_TIME 10

using namespace std;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(MIN_TIME, MAX_TIME);

struct ClientParams {
    string clientName;
    int id;
};

struct BarberParams {
    string barberName;
    int barberStamina;
};

HANDLE printingSemaphore;
HANDLE chairSemaphore;
HANDLE barberSemaphore;
HANDLE barberStarterSemaphore;

HANDLE queueSemaphore;
vector<HANDLE> customersSemaphores;

LinkedQueue linkedQueue;
int queue_max = 0;

void synchronizedPrint(const string &msg) {
    WaitForSingleObject(printingSemaphore, INFINITE);
    cout << msg << endl;
    ReleaseSemaphore(printingSemaphore, 1, nullptr);
}

void barbering(const string &barberName, const int &barberStamina) {
    for (int i = 0; i < barberStamina; ++i) {

        if (linkedQueue.size() == 0) {
            synchronizedPrint("'" + barberName + "': There are no clients. I go sleep...");
            WaitForSingleObject(barberSemaphore, INFINITE);
        }

        WaitForSingleObject(queueSemaphore, INFINITE);
        int customerId = stoi(*linkedQueue.front());
        linkedQueue.pop();
        ReleaseSemaphore(queueSemaphore, 1, nullptr);

        string customerName = "Client " + to_string(customerId);
        string msg;
        msg.append("'").append(barberName).append("': Hi '").append(customerName).append("'! Come to my room.");
        synchronizedPrint(msg);

        ReleaseSemaphore(customersSemaphores[customerId], 1, nullptr);

        WaitForSingleObject(barberStarterSemaphore, INFINITE);

        double seconds = dist(mt);
        synchronizedPrint("'" + barberName + "': I am working for " + to_string(seconds) + " seconds!");
        sleep(seconds);

        ReleaseSemaphore(chairSemaphore, 1, nullptr);

    }
    synchronizedPrint("'" + barberName + "': I am tired, I go home!");
}

DWORD WINAPI barberingRoutine(LPVOID lpParams) {
    BarberParams bp = *(BarberParams *) lpParams;
    barbering(bp.barberName, bp.barberStamina);
    return 0;
}

void hairCut(const string &clientName, const int &clientId) {
    synchronizedPrint("'" + clientName + "' enters to barbary...");

    WaitForSingleObject(queueSemaphore, INFINITE);
    if (linkedQueue.size() == 0) {
        linkedQueue.push(to_string(clientId));
        ReleaseSemaphore(barberSemaphore, 1, nullptr);
    } else if (linkedQueue.size() == queue_max) {
        return;
    } else {
        linkedQueue.push(to_string(clientId));
    }
    ReleaseSemaphore(queueSemaphore, 1, nullptr);

    WaitForSingleObject(customersSemaphores[clientId], INFINITE);

    double seconds = dist(mt);
    synchronizedPrint("'" + clientName + "': Hi barber let me " + to_string(seconds) + " seconds!");
    sleep(seconds);

    ReleaseSemaphore(barberStarterSemaphore, 1, nullptr);

    WaitForSingleObject(chairSemaphore, INFINITE);

    WaitForSingleObject(barberSemaphore, INFINITE);
    synchronizedPrint("'" + clientName + "' lefts barbary");
}

DWORD WINAPI hairCutRoutine(LPVOID lpParams) {
    ClientParams clientParams = *(ClientParams *) lpParams;
    for (int i = 0; i < 2; ++i) {
        hairCut(clientParams.clientName, clientParams.id);
        double seconds = dist(mt);
        sleep(10 * seconds);
    }
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
    queueSemaphore = CreateSemaphoreA(nullptr, 1, 1, nullptr);
    printingSemaphore = CreateSemaphoreA(nullptr, 1, 1, nullptr);
    barberSemaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
    chairSemaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
    barberStarterSemaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
    queue_max = clientChairsCount;

    for (int i = 0; i < clientCount; ++i) {
        HANDLE semaphore = CreateSemaphoreA(nullptr, 0, 1, nullptr);
        customersSemaphores.push_back(semaphore);
    }

    BarberParams bp{"Barber", barberStamina};
    threads[0] = CreateThread(nullptr, 0, barberingRoutine, &bp, 0, nullptr);

    sleep(dist(mt));

    for (int i = 0; i < clientCount; ++i) {
        string clientName = "Client " + to_string(i);
        cp[i].clientName = clientName;
        cp[i].id = i;

        threads[i + 1] = CreateThread(nullptr, 0, hairCutRoutine, &cp[i], 0, nullptr);
    }

    WaitForMultipleObjects(clientCount + 1, threads, TRUE, INFINITE);

    for (HANDLE thread: threads) {
        CloseHandle(thread);
    }

    for (HANDLE semaphore: customersSemaphores) {
        CloseHandle(semaphore);
    }

    CloseHandle(barberStarterSemaphore);
    CloseHandle(chairSemaphore);
    CloseHandle(barberSemaphore);
    CloseHandle(printingSemaphore);
    CloseHandle(queueSemaphore);

    return 0;
}
