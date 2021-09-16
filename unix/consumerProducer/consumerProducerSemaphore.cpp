#include <iostream>
#include <queue>
#include <unistd.h>
#include "semaphore.h"

using namespace std;

struct producerRoutineParams {
    string producerName;
    int productsCount;
};

struct consumerRoutineParams {
    string customerName;
    int productsCount;
};

Semaphore produceSemaphore;
Semaphore consumeSemaphore;
pthread_mutex_t coutMtx;

int productCounter = 0;
queue<string> productsQueue;

void synchronizedPrint(const string &msg) {
    pthread_mutex_lock(&coutMtx);
    cout << msg << endl;
    pthread_mutex_unlock(&coutMtx);
}

void rest(const string &humanName, const int &seconds) {
    synchronizedPrint("Human '" + humanName + "' go rest for " + to_string(seconds) + " seconds!");
    sleep(seconds);
}

void insertProductInQueue(const string &producerName, int iteration) {
    string msg = "'" + producerName + "' has iteration " + to_string(iteration) + " and produces 'Product-" +
                 to_string(productCounter) +
                 "'. Wait 2 seconds!";
    synchronizedPrint(msg);
    sleep(2);
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

void *produceRoutine(void *params) {
    producerRoutineParams producerData = *(producerRoutineParams *) params;

    produce(producerData.producerName, producerData.productsCount);

    pthread_exit(nullptr);
}

void popProductFromQueue(const string &customerName, int iteration) {
    string consumedProduct = productsQueue.front();

    string msg = "'" + customerName + "' has iteration " + to_string(iteration) +
                 " and consume '" + consumedProduct + "'. Wait 2 seconds!";
    synchronizedPrint(msg);
    sleep(2);
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

void *consumeRoutine(void *params) {
    consumerRoutineParams consumerData = *(consumerRoutineParams *) params;

    consume(consumerData.customerName, consumerData.productsCount);

    pthread_exit(nullptr);
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

    cout << "Enter products count for each producer/consumer: ";
    cin >> productsCount;

    cout << "Enter producers count: ";
    cin >> producersCount;

    cout << "Enter customers count: ";
    cin >> customersCount;

    initQueue();
    int peopleCount = producersCount + customersCount;
    pthread_t people[peopleCount];
    consumerRoutineParams consumerData[customersCount];
    producerRoutineParams producerData[peopleCount];
    pthread_mutex_init(&coutMtx, nullptr);

    for (int i = 0; i < producersCount; ++i) {
        string producerName = "Producer " + to_string(i);
        producerData[i].producerName = producerName;
        producerData[i].productsCount = productsCount;

        pthread_create(&people[i], nullptr, &produceRoutine, &producerData[i]);
    }

    for (int i = 0; i < customersCount; ++i) {
        string customerName = "Customer " + to_string(i);
        consumerData[i].customerName = customerName;
        consumerData[i].productsCount = productsCount;

        pthread_create(&people[producersCount + i], nullptr, &consumeRoutine, &consumerData[i]);
    }

    for (pthread_t t: people) {
        pthread_join(t, nullptr);
    }

    pthread_mutex_destroy(&coutMtx);
    pthread_exit(nullptr);
}
