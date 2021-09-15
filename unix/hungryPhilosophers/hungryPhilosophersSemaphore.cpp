#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

vector<bool> forks;

Semaphore editSemaphore;
Semaphore printSemaphore;

struct philosopherData {
    string philosopherName;
    int position;
    int eatCount;
};

void synchronizedPrint(const string &msg) {
    printSemaphore.acquire();
    cout << msg << endl;
    printSemaphore.release();
}

void think(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes think for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

void eat(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes eat for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

bool isForkAvailable(const int &leftFork, const int &rightFork) {
    return !forks[leftFork] && !forks[rightFork];
}

bool takeForks(int leftFork, int rightFork) {
    if (editSemaphore.try_acquire()) {
        if (!forks[leftFork] && !forks[rightFork]) {
            forks[leftFork] = true;
            forks[rightFork] = true;
            editSemaphore.release();
            return true;
        }
        editSemaphore.release();
    }

    return false;
}

void dropForks(int leftFork, int rightFork) {
    editSemaphore.acquire();
    forks[leftFork] = false;
    forks[rightFork] = false;
    editSemaphore.release();
}

void execute(const string &threadName, const int &philosopherPosition, const int &eatCountMax) {
    int leftFork = philosopherPosition;
    int rightFork;

    if (philosopherPosition == forks.size() - 1) {
        rightFork = 0;
    } else {
        rightFork = philosopherPosition + 1;
    }

    int eatCount = 0;
    while (eatCount < eatCountMax) {
        bool canEat = takeForks(leftFork, rightFork);
        if (canEat) {
            eat(threadName, 2);
            dropForks(leftFork, rightFork);
            think(threadName, 6);
            eatCount++;
        } else {
            think(threadName, 3);
        }
    }

    synchronizedPrint("'" + threadName + "' is full!");
}

void *executeRoutine(void *params) {
    philosopherData pd = *(philosopherData *) params;

    execute(pd.philosopherName, pd.position, pd.eatCount);

    pthread_exit(nullptr);
}

int main() {
    int philosophersCount;
    int eatCount;

    cout << "Enter philosophers count: ";
    cin >> philosophersCount;

    cout << "Enter eat count: ";
    cin >> eatCount;

    forks = vector<bool>(philosophersCount, false);
    pthread_t philosophers[philosophersCount];
    philosopherData pd[philosophersCount];

    for (int i = 0; i < philosophersCount; ++i) {
        string philosopherName = "Philosopher " + to_string(i);
        pd[i].philosopherName = philosopherName;
        pd[i].position = i;
        pd[i].eatCount = eatCount;

        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_create(&philosophers[i], &attr, &executeRoutine, &pd[i]);
    }

    for (pthread_t philosopher: philosophers) {
        pthread_join(philosopher, nullptr);
    }

    pthread_exit(nullptr);
}
