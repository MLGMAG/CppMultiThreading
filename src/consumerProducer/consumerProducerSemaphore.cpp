#include <thread>
#include <iostream>
#include <queue>
#include <vector>
#include "semaphore.h"

using namespace std;

Semaphore produceSemaphore(1);
Semaphore consumeSemaphore(1);
mutex coutMtx;

int productCounter = 0;
queue<string> productsQueue;

void synchronizedPrint(const string &msg) {
    lock_guard guard(coutMtx);
    cout << msg << endl;
}

void rest(const string &humanName, const int &seconds) {
    synchronizedPrint("Human '" + humanName + "' go rest for " + to_string(seconds) + " seconds!");
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void insertProductInQueue(const string &producerName, int iteration) {
    string msg = "'" + producerName + "' has iteration " + to_string(iteration) + " and produces 'Product-" +
                 to_string(productCounter) +
                 "'. Wait 2 seconds!";
    synchronizedPrint(msg);
    this_thread::sleep_for(chrono::seconds(2));
    productsQueue.push("Product-" + to_string(productCounter));
    productCounter++;
}

void produceProduct(const string &producerName, int iteration) {
    while (true) {
        if (produceSemaphore.try_acquire()) {
            insertProductInQueue(producerName, iteration);
            produceSemaphore.release();
            rest(producerName, 6);
            break;
        } else {
            rest(producerName, 4);
        }
    }
}

void produce(const string &producerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        produceProduct(producerName, i);
    }
    synchronizedPrint("'" + producerName + "' is tired!");
}

void popProductFromQueue(const string &customerName, int iteration) {
    string consumedProduct = productsQueue.front();

    string msg = "'" + customerName + "' has iteration " + to_string(iteration) +
                 " and consume '" + consumedProduct + "'. Wait 2 seconds!";
    synchronizedPrint(msg);
    this_thread::sleep_for(chrono::seconds(2));
    productsQueue.pop();
}

void consumeProduct(const string &customerName, int iteration) {
    while (true) {
        if (consumeSemaphore.try_acquire()) {
            if (productsQueue.empty()) {
                consumeSemaphore.release();
                rest(customerName, 4);
                continue;
            }
            popProductFromQueue(customerName, iteration);
            consumeSemaphore.release();
            rest(customerName, 6);
            break;
        } else {
            rest(customerName, 4);
        }
    }
}

void consume(const string &customerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        consumeProduct(customerName, i);
    }
    synchronizedPrint("'" + customerName + "' is tired!");
}

void initQueue() {
    productsQueue.emplace("Old item 1");
    productsQueue.emplace("Old item 2");
    productsQueue.emplace("Old item 3");
}

int main() {
    int productsCount;
    int producersCount;
    int customersCount;

    cout << "Enter products count: ";
    cin >> productsCount;

    cout << "Enter producers count: ";
    cin >> producersCount;

    cout << "Enter customers count: ";
    cin >> customersCount;

    initQueue();
    vector<thread> people;
    for (int i = 0; i < producersCount; ++i) {
        string producerName = "Producer " + to_string(i);
        people.emplace_back(produce, producerName, productsCount);
    }

    for (int i = 0; i < customersCount; ++i) {
        string customerName = "Customer " + to_string(i);
        people.emplace_back(consume, customerName, productsCount);
    }

    for (thread &t: people) {
        t.join();
    }
}
