#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

vector<bool> forks;

mutex forkMtx;
mutex coutMtx;

void synchronizedPrint(const string &msg) {
    lock_guard guard(coutMtx);
    cout << msg << endl;
}

void think(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' go think for " + to_string(seconds) + " seconds");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void eat(const string &threadName, const int &seconds) {
    synchronizedPrint("'" + threadName + "' go eat for " + to_string(seconds) + " seconds");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

bool takeForks(int leftFork, int rightFork) {
    lock_guard guard(forkMtx);

    if (!forks[leftFork] && !forks[rightFork]) {
        forks[leftFork] = true;
        forks[rightFork] = true;
        return true;
    }

    return false;
}

void dropForks(int leftFork, int rightFork) {
    lock_guard guard(forkMtx);
    forks[leftFork] = false;
    forks[rightFork] = false;
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
            eat(threadName, 5);
            dropForks(leftFork, rightFork);
            think(threadName, 2);
            eatCount++;
        }
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
