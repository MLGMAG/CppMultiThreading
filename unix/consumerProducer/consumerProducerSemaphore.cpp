#include <iostream>
#include <unistd.h>
#include <vector>
#include "semaphore.h"
#include "linkedQueue.h"
#include <random>

#define MIN_TIME 1
#define MAX_TIME 10

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
pthread_mutex_t consumerMtx;
pthread_mutex_t producerMtx;

int productCounter = 0;
LinkedQueue linkedQueue;

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(MIN_TIME, MAX_TIME);


template<class T>
void synchronizedPrint(const T &msg) {
    pthread_mutex_lock(&coutMtx);
    cout << msg << endl;
    pthread_mutex_unlock(&coutMtx);
}

template<class T>
void synchronizedPrintMsgAndQueue(const T &msg) {
    pthread_mutex_lock(&coutMtx);
    cout << msg << linkedQueue << endl;
    pthread_mutex_unlock(&coutMtx);
}

void rest(const string &humanName) {
    double seconds = dist(mt);

    synchronizedPrint("Human '" + humanName + "' go rest for " + to_string(seconds) + " seconds!");
    sleep(seconds);
}

void insertProductInQueue(const string &producerName, const int &iteration) {
    double seconds = dist(mt);
    string msg = "'" + producerName + "' has iteration " + to_string(iteration) + " and produces 'Product-" +
                 to_string(productCounter) +
                 "'. Wait " + to_string(seconds) + " seconds!";
    synchronizedPrint(msg);
    sleep(seconds);
    linkedQueue.push("Product-" + to_string(productCounter));
    productCounter++;
}

void produceProduct(const string &producerName, const int &iteration) {
    pthread_mutex_lock(&producerMtx);
    if (linkedQueue.size() < 2) {
        pthread_mutex_lock(&consumerMtx);

        produceSemaphore.acquire();
        synchronizedPrintMsgAndQueue(producerName + ". Start. Queue: ");
        insertProductInQueue(producerName, 0);
        consumeSemaphore.release();

        produceSemaphore.acquire();
        synchronizedPrintMsgAndQueue(producerName + ". Queue: ");
        insertProductInQueue(producerName, 0);
        consumeSemaphore.release();

        pthread_mutex_unlock(&consumerMtx);
    } else {
        produceSemaphore.acquire();
        synchronizedPrintMsgAndQueue(producerName + ". Start. Queue: ");
        insertProductInQueue(producerName, iteration);
        consumeSemaphore.release();
    }
    synchronizedPrintMsgAndQueue(producerName + ". End. Queue : ");
    pthread_mutex_unlock(&producerMtx);
}

void produce(const string &producerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        produceProduct(producerName, i);
        rest(producerName);
    }
    synchronizedPrint("'" + producerName + "' is tired!");
}

void *produceRoutine(void *params) {
    producerRoutineParams producerData = *(producerRoutineParams *) params;

    produce(producerData.producerName, producerData.productsCount);

    pthread_exit(nullptr);
}

void popProductFromQueue(const string &customerName, const int &iteration) {
    string consumedProduct = *linkedQueue.front();

    string msg = "'" + customerName + "' has iteration " + to_string(iteration) +
                 " and consume '" + consumedProduct + "'. Wait 2 seconds!";
    synchronizedPrint(msg);
    sleep(2);
    linkedQueue.pop();
}

void consumeProduct(const string &customerName, const int &iteration) {
    consumeSemaphore.acquire();
    pthread_mutex_lock(&consumerMtx);
    synchronizedPrintMsgAndQueue(customerName + ". Start. Queue: ");

    popProductFromQueue(customerName, iteration);
    produceSemaphore.release();

    synchronizedPrintMsgAndQueue(customerName + ". End. Queue: ");
    pthread_mutex_unlock(&consumerMtx);
}

void consume(const string &customerName, const int &productsCount) {
    for (int i = 0; i < productsCount; ++i) {
        consumeProduct(customerName, i);
        rest(customerName);
        rest(customerName);
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
    pthread_mutex_init(&coutMtx, nullptr);
    pthread_mutex_init(&consumerMtx, nullptr);
    pthread_mutex_init(&producerMtx, nullptr);

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

    pthread_mutex_destroy(&producerMtx);
    pthread_mutex_destroy(&consumerMtx);
    pthread_mutex_destroy(&coutMtx);
    pthread_exit(nullptr);
}
