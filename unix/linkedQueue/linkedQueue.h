#pragma once

#include <iostream>
#include <cstdlib>
#include <string>

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

//    void print() const;

private:
    int length = 0;
    Node *head = nullptr;
    Node *tail = nullptr;
};

std::ostream &operator<<(std::ostream &os, const LinkedQueue &queue);
