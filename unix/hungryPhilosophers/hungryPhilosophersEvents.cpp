#include <iostream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <unistd.h>
#include <random>

#define MIN_TIME 1
#define MAX_TIME 10

using namespace std;

vector<pthread_mutex_t> forks_mutexes;
vector<pthread_mutex_t> philosophers_mutexes;
vector<pthread_cond_t> forks_conditions;
pthread_mutex_t printMtx;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(MIN_TIME, MAX_TIME);

struct philosopherData {
    string philosopherName;
    int position = 0;
    int eatCount = 0;
};

struct philosopherParams {
    string threadName;
    pthread_mutex_t &leftFork;
    pthread_mutex_t &rightFork;
    pthread_mutex_t &philosopherMutex;
    pthread_cond_t &leftForkCond;
    pthread_cond_t &rightForkCond;
};

void takeForksOnLeft(philosopherParams &phParam);

void takeForksOnRight(philosopherParams &phParam);


void synchronizedPrint(const string &msg) {
    pthread_mutex_lock(&printMtx);
    cout << msg << endl;
    pthread_mutex_unlock(&printMtx);
}

void think(const string &threadName) {
    double seconds = dist(mt);

    synchronizedPrint("'" + threadName + "' goes think for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

void eat(const string &threadName) {
    double seconds = dist(mt);

    synchronizedPrint("'" + threadName + "' goes eat for " + to_string(seconds) + " seconds");
    sleep(seconds);
}

void takeForksOnRight(philosopherParams &phParam) {
    pthread_mutex_lock(&phParam.philosopherMutex);

    while (pthread_mutex_trylock(&phParam.rightFork) != 0) {
        synchronizedPrint("'" + phParam.threadName + "' can NOT takes RIGHT Fork and waits for it...");
        pthread_cond_wait(&phParam.rightForkCond, &phParam.philosopherMutex);
    }

    synchronizedPrint("'" + phParam.threadName + "' successfully takes RIGHT Fork");

    if (pthread_mutex_trylock(&phParam.leftFork) != 0) {
        pthread_mutex_unlock(&phParam.rightFork);
        pthread_cond_signal(&phParam.rightForkCond);
        pthread_mutex_unlock(&phParam.philosopherMutex);

        takeForksOnLeft(phParam);
    } else {
        synchronizedPrint("'" + phParam.threadName + "' successfully takes LEFT Fork");
        eat(phParam.threadName);
        pthread_mutex_unlock(&phParam.leftFork);
        pthread_mutex_unlock(&phParam.rightFork);
        pthread_cond_signal(&phParam.leftForkCond);
        pthread_cond_signal(&phParam.rightForkCond);
        pthread_mutex_unlock(&phParam.philosopherMutex);
    }
}

void takeForksOnLeft(philosopherParams &phParam) {
    pthread_mutex_lock(&phParam.philosopherMutex);
    while (pthread_mutex_trylock(&phParam.leftFork) != 0) {
        synchronizedPrint("'" + phParam.threadName + "' can NOT takes LEFT Fork and waits for it...");
        pthread_cond_wait(&phParam.leftForkCond, &phParam.philosopherMutex);
    }

    synchronizedPrint("'" + phParam.threadName + "' successfully takes LEFT Fork");

    if (pthread_mutex_trylock(&phParam.rightFork) != 0) {
        pthread_mutex_unlock(&phParam.leftFork);
        pthread_cond_signal(&phParam.leftForkCond);
        pthread_mutex_unlock(&phParam.philosopherMutex);

        takeForksOnRight(phParam);
    } else {
        synchronizedPrint("'" + phParam.threadName + "' successfully takes RIGHT Fork");
        eat(phParam.threadName);
        pthread_mutex_unlock(&phParam.rightFork);
        pthread_mutex_unlock(&phParam.leftFork);
        pthread_cond_signal(&phParam.rightForkCond);
        pthread_cond_signal(&phParam.leftForkCond);
        pthread_mutex_unlock(&phParam.philosopherMutex);
    }
}

void execute(const string &threadName, const int &philosopherPosition, const int &eatCountMax) {
    int leftFork = philosopherPosition;
    int rightFork;

    if (philosopherPosition == forks_mutexes.size() - 1) {
        rightFork = 0;
    } else {
        rightFork = philosopherPosition + 1;
    }

    philosopherParams phParam{threadName,
                              forks_mutexes[leftFork],
                              forks_mutexes[rightFork],
                              philosophers_mutexes[philosopherPosition],
                              forks_conditions[leftFork],
                              forks_conditions[rightFork]};

    for (int i = 0; i < eatCountMax; ++i) {
        takeForksOnLeft(phParam);
        think(threadName);
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
        forks_mutexes.push_back(mutex);
    }

    for (int i = 0; i < philosophersCount; ++i) {
        pthread_mutex_t mutex;
        pthread_mutex_init(&mutex, nullptr);
        philosophers_mutexes.push_back(mutex);
    }

    for (int i = 0; i < philosophersCount; ++i) {
        pthread_cond_t cond;
        pthread_cond_init(&cond, nullptr);
        forks_conditions.push_back(cond);
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

    for (pthread_cond_t cond: forks_conditions) {
        pthread_cond_destroy(&cond);
    }

    for (pthread_mutex_t mutex: philosophers_mutexes) {
        pthread_mutex_destroy(&mutex);
    }

    for (pthread_mutex_t mutex: forks_mutexes) {
        pthread_mutex_destroy(&mutex);
    }

    pthread_mutex_destroy(&printMtx);
    pthread_exit(nullptr);
}
