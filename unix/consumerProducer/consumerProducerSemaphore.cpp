#include <iostream>
#include <unistd.h>
#include <vector>
#include "semaphore.h"
#include "linkedQueue.h"
#include <random>

#define MIN_TIME 1
#define MAX_TIME 5

using namespace std;

struct producerRoutineParams {
    string producerName;
    int productsCount{};
};

struct consumerRoutineParams {
    string customerName;
    int productsCount{};
};

Semaphore produceSemaphore;
Semaphore consumeSemaphore;
Semaphore consumerLockSemaphore;
Semaphore producerLockSemaphore;
Semaphore printSemaphore;
Semaphore queueSemaphore;

int productCounter = 0;
LinkedQueue linkedQueue;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(MIN_TIME, MAX_TIME);


template<class T>
void synchronizedPrint(const T &msg) {
    printSemaphore.acquire();
    cout << msg << endl;
    printSemaphore.release();
}

template<class T>
void synchronizedPrintMsgAndQueue(const T &msg) {
    printSemaphore.acquire();
    cout << msg << linkedQueue << endl;
    printSemaphore.release();
}

void insertProductInQueue(const string &producerName) {
    sleep(dist(mt));
    linkedQueue.push("Product-" + to_string(productCounter));
    productCounter++;
    synchronizedPrintMsgAndQueue(producerName + ". End. Queue : ");
}

void produceProduct(const string &producerName) {
    producerLockSemaphore.acquire();
    produceSemaphore.acquire();

    queueSemaphore.acquire();
    if (linkedQueue.size() == 1) {
        insertProductInQueue(producerName);
        queueSemaphore.release();
    } else {
        queueSemaphore.release();
        insertProductInQueue(producerName);
    }

    consumeSemaphore.release();
    producerLockSemaphore.release();
}

void produce(const string &producerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        produceProduct(producerName);
        sleep(dist(mt));
    }
    synchronizedPrint("'" + producerName + "' is tired!");
}

void *produceRoutine(void *params) {
    producerRoutineParams producerData = *(producerRoutineParams *) params;

    produce(producerData.producerName, producerData.productsCount);

    pthread_exit(nullptr);
}

void popProductFromQueue(const string &customerName) {
    sleep(dist(mt));
    linkedQueue.pop();
    synchronizedPrintMsgAndQueue(customerName + ". End. Queue : ");
}

void consumeProduct(const string &customerName) {
    consumerLockSemaphore.acquire();
    consumeSemaphore.acquire();

    queueSemaphore.acquire();
    if (linkedQueue.size() == 1) {
        popProductFromQueue(customerName);
        queueSemaphore.release();
    } else {
        queueSemaphore.release();
        popProductFromQueue(customerName);
    }

    produceSemaphore.release();
    consumerLockSemaphore.release();
}

void consume(const string &customerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        consumeProduct(customerName);
        sleep(dist(mt));
    }
    synchronizedPrint("'" + customerName + "' is tired!");
}

void *consumeRoutine(void *params) {
    consumerRoutineParams consumerData = *(consumerRoutineParams *) params;

    consume(consumerData.customerName, consumerData.productsCount);

    pthread_exit(nullptr);
}

int main() {
    int bufferSize;
    int productsCount;
    int producersCount;
    int customersCount;

    cout << "Enter buffer size: ";
    cin >> bufferSize;

    cout << "Enter products count for each producer/consumer: ";
    cin >> productsCount;

    cout << "Enter producers count: ";
    cin >> producersCount;

    cout << "Enter customers count: ";
    cin >> customersCount;

    consumeSemaphore = Semaphore(bufferSize, 0);
    produceSemaphore = Semaphore(bufferSize, bufferSize);
    int peopleCount = producersCount + customersCount;
    vector<pthread_t> people;
    consumerRoutineParams consumerData[customersCount];
    producerRoutineParams producerData[peopleCount];

    for (int i = 0; i < producersCount; ++i) {
        string producerName = "Producer " + to_string(i);
        producerData[i].producerName = producerName;
        producerData[i].productsCount = productsCount;

        pthread_t thread;
        pthread_create(&thread, nullptr, &produceRoutine, &producerData[i]);
        people.push_back(thread);
    }

    for (int i = 0; i < customersCount; ++i) {
        string customerName = "Customer " + to_string(i);
        consumerData[i].customerName = customerName;
        consumerData[i].productsCount = productsCount;

        pthread_t thread;
        pthread_create(&thread, nullptr, &consumeRoutine, &consumerData[i]);
        people.push_back(thread);
    }

    for (pthread_t t: people) {
        pthread_join(t, nullptr);
    }

    pthread_exit(nullptr);
}
