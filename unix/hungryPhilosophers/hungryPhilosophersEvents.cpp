#include <iostream>
#include <vector>
#include <pthread.h>
#include <mutex>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

vector<bool> forks;

pthread_mutex_t forkMtx;
pthread_cond_t forkCond;
pthread_mutex_t printMtx;

struct philosopherData {
    string philosopherName;
    int position;
    int eatCount;
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

bool isForkAvailable(const int &leftFork, const int &rightFork) {
    return !forks[leftFork] && !forks[rightFork];
}

timespec getThinkTime() {
    struct timespec to;
    struct timeval now;
    gettimeofday(&now, nullptr);
    to.tv_sec = time(nullptr) + 3;
    to.tv_nsec = 0;

    return to;
}

void takeForks(const string &threadName, int leftFork, int rightFork) {
    pthread_mutex_lock(&forkMtx);

    while (!isForkAvailable(leftFork, rightFork)) {
        timespec to = getThinkTime();
        synchronizedPrint("'" + threadName + "' goes think for 3 seconds");
        pthread_cond_timedwait(&forkCond, &forkMtx, &to);
    }

    forks[leftFork] = true;
    forks[rightFork] = true;

    pthread_cond_broadcast(&forkCond);

    pthread_mutex_unlock(&forkMtx);
}

void dropForks(int leftFork, int rightFork) {
    pthread_mutex_lock(&forkMtx);
    forks[leftFork] = false;
    forks[rightFork] = false;
    pthread_mutex_unlock(&forkMtx);
    pthread_cond_broadcast(&forkCond);
}

void execute(const string &threadName, const int &philosopherPosition, const int &eatCountMax) {
    int leftFork = philosopherPosition;
    int rightFork;

    if (philosopherPosition == forks.size() - 1) {
        rightFork = 0;
    } else {
        rightFork = philosopherPosition + 1;
    }

    for (int i = 0; i < eatCountMax; ++i) {
        takeForks(threadName, leftFork, rightFork);
        eat(threadName, 2);
        dropForks(leftFork, rightFork);
        think(threadName, 6);
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
    pthread_mutex_init(&printMtx, nullptr);
    pthread_mutex_init(&forkMtx, nullptr);
    pthread_cond_init(&forkCond, nullptr);

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

    pthread_cond_destroy(&forkCond);
    pthread_mutex_destroy(&forkMtx);
    pthread_mutex_destroy(&printMtx);
    pthread_exit(nullptr);
}
