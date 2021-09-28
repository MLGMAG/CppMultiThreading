#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <pthread.h>

class Node {
public:
    Node() = default;

    Node *next = nullptr;
    Node *prev = nullptr;
    std::string data{};
};

class LinkedQueue {
public:

    LinkedQueue();

    ~LinkedQueue();

    int size() const;

    Node *getHead() const;

    void push(const std::string_view &data);

    void pop();

    void clear();

    std::string *front() const;

    std::string *back() const;

private:
    int length = 0;
    Node *head = nullptr;
    Node *tail = nullptr;
    pthread_mutex_t mutex;

    void init();
};

std::ostream &operator<<(std::ostream &os, const LinkedQueue &queue);
