#include <iostream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <unistd.h>

using namespace std;

void takeForksOnLeft(const string &threadName, pthread_mutex_t &leftFork, pthread_mutex_t &rightFork);

void takeForksOnRight(const string &threadName, pthread_mutex_t &leftFork, pthread_mutex_t &rightFork);

vector<pthread_mutex_t> forks;

pthread_mutex_t printMtx;

struct philosopherData {
    string philosopherName;
    int position = 0;
    int eatCount = 0;
};

void synchronizedPrint(const string &msg) {
    pthread_mutex_lock(&printMtx);
    cout << msg << endl;
    pthread_mutex_unlock(&printMtx);
}

void think(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes think for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

void eat(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes eat for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

void takeForksOnRight(const string &threadName, pthread_mutex_t &leftFork, pthread_mutex_t &rightFork) {
    pthread_mutex_lock(&rightFork);
    synchronizedPrint("'" + threadName + "' successfully takes RIGHT Fork");

    bool takeLeftFork = pthread_mutex_trylock(&leftFork) == 0;
    if (takeLeftFork) {
        synchronizedPrint("'" + threadName + "' successfully takes LEFT Fork");
        eat(threadName, 10);
        pthread_mutex_unlock(&leftFork);
        pthread_mutex_unlock(&rightFork);
    } else {
        synchronizedPrint("'" + threadName + "' can NOT takes LEFT Fork and waits for it...");
        pthread_mutex_unlock(&rightFork);
        takeForksOnLeft(threadName, leftFork, rightFork);
    }
}

void takeForksOnLeft(const string &threadName, pthread_mutex_t &leftFork, pthread_mutex_t &rightFork) {
    pthread_mutex_lock(&leftFork);
    synchronizedPrint("'" + threadName + "' successfully takes LEFT Fork");

    bool takeRightFork = pthread_mutex_trylock(&rightFork) == 0;
    if (takeRightFork) {
        synchronizedPrint("'" + threadName + "' successfully takes RIGHT Fork");
        eat(threadName, 10);
        pthread_mutex_unlock(&rightFork);
        pthread_mutex_unlock(&leftFork);
    } else {
        synchronizedPrint("'" + threadName + "' can NOT takes RIGHT Fork and waits for it...");
        pthread_mutex_unlock(&leftFork);
        takeForksOnRight(threadName, leftFork, rightFork);
    }
}

void takeForks(const string &threadName, pthread_mutex_t &leftFork, pthread_mutex_t &rightFork) {
    int takeLeftFork = pthread_mutex_trylock(&leftFork);

    if (takeLeftFork == 0) {
        synchronizedPrint("'" + threadName + "' successfully takes LEFT Fork");

        int takeRightFork = pthread_mutex_trylock(&rightFork);

        if (takeRightFork == 0) {
            synchronizedPrint("'" + threadName + "' successfully takes RIGHT Fork");
            eat(threadName, 10);
            pthread_mutex_unlock(&rightFork);
            pthread_mutex_unlock(&leftFork);
        } else {
            pthread_mutex_unlock(&leftFork);
            synchronizedPrint("'" + threadName + "' can NOT takes RIGHT Fork and waits for it...");
            takeForksOnRight(threadName, leftFork, rightFork);
        }
    } else {
        synchronizedPrint("'" + threadName + "' can NOT takes LEFT Fork and waits for it...");
        takeForksOnLeft(threadName, leftFork, rightFork);
    }

    think(threadName, 6);
}

void execute(const string &threadName, const int &philosopherPosition, const int &eatCountMax) {
    int leftFork = philosopherPosition;
    int rightFork;

    if (philosopherPosition == forks.size() - 1) {
        rightFork = 0;
    } else {
        rightFork = philosopherPosition + 1;
    }

    pthread_mutex_t &leftForkMutex = forks[leftFork];
    pthread_mutex_t &rightForkMutex = forks[rightFork];

    for (int i = 0; i < eatCountMax; ++i) {
        takeForks(threadName, leftForkMutex, rightForkMutex);
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

    for (int i = 0; i < philosophersCount; ++i) {
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, nullptr);
        forks.push_back(mutex);
    }

    vector<pthread_t> philosophers;
    philosopherData pd[philosophersCount];
    pthread_mutex_init(&printMtx, nullptr);

    for (int i = 0; i < philosophersCount; ++i) {
        string philosopherName = "Philosopher " + to_string(i);
        pd[i].philosopherName = philosopherName;
        pd[i].position = i;
        pd[i].eatCount = eatCount;

        pthread_t philosopher;
        pthread_create(&philosopher, nullptr, &executeRoutine, &pd[i]);
        philosophers.push_back(philosopher);
    }

    for (pthread_t philosopher: philosophers) {
        pthread_join(philosopher, nullptr);
    }

    for (pthread_mutex_t mutex: forks) {
        pthread_mutex_destroy(&mutex);
    }
    pthread_mutex_destroy(&printMtx);
    pthread_exit(nullptr);
}
