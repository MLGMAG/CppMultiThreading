#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

vector<bool> forks;

mutex forkMtx;
mutex coutMtx;
condition_variable cv;

void synchronizedPrint(const string &msg) {
    lock_guard guard(coutMtx);
    cout << msg << endl;
}

void think(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes think for " + to_string(seconds) + " seconds");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void eat(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' goes eat for " + to_string(seconds) + " seconds");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void takeForks(const string &threadName, int leftFork, int rightFork) {
    unique_lock ul(forkMtx);

    synchronizedPrint("'" + threadName + "' tries to take fork!");
    auto isForkAvailable = [leftFork, rightFork]() { return !forks[leftFork] && !forks[rightFork]; };
    bool canTakeForks = cv.wait_for(ul, chrono::seconds(3), isForkAvailable);

    if (canTakeForks) {
        forks[leftFork] = true;
        forks[rightFork] = true;
        cv.notify_all();
    } else {
        think(threadName, 3);
    }
}

void dropForks(int leftFork, int rightFork) {
    lock_guard guard(forkMtx);
    forks[leftFork] = false;
    forks[rightFork] = false;
    cv.notify_all();
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

int main() {
    int philosophersCount;
    int eatCount;

    cout << "Enter philosophers count: ";
    cin >> philosophersCount;

    cout << "Enter eat count: ";
    cin >> eatCount;

    forks = vector<bool>(philosophersCount, false);
    vector<thread> philosophers;

    for (int i = 0; i < philosophersCount; ++i) {
        string threadName = "Philosopher " + to_string(i);
        philosophers.emplace_back(execute, threadName, i, eatCount);
    }

    for (thread &t: philosophers) {
        t.join();
    }

}
