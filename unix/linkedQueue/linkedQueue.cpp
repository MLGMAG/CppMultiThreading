#include "linkedQueue.h"

using namespace std;

LinkedQueue::LinkedQueue() {
    init();
}

LinkedQueue::~LinkedQueue() {
    clear();
    pthread_mutex_destroy(&mutex);
}

void LinkedQueue::init() {
    pthread_mutex_init(&mutex, nullptr);
}

int LinkedQueue::size() const {
    return length;
}

Node *LinkedQueue::getHead() const {
    return head;
}

void LinkedQueue::push(const string_view &data) {
    if (length == 0) {
        auto *node = new Node;
        node->data = data;
        node->next = nullptr;
        node->prev = nullptr;
        this->head = node;
        this->tail = node;
    } else {
        Node *oldTail = this->tail;

        auto *newTail = new Node;
        newTail->data = data;
        newTail->next = oldTail;
        newTail->prev = nullptr;

        oldTail->prev = newTail;

        this->tail = newTail;
    }
    pthread_mutex_lock(&mutex);
    length++;
    pthread_mutex_unlock(&mutex);
}

void LinkedQueue::pop() {
    if (length == 0) {
        return;
    }

    if (length == 1) {
        Node *oldHead = head;
        head = nullptr;
        tail = nullptr;
        delete oldHead;

        pthread_mutex_lock(&mutex);
        length--;
        pthread_mutex_unlock(&mutex);
        return;
    }

    if (length == 2 && tail != nullptr) {
        Node *oldHead = head;

        tail->next = nullptr;
        head = tail;
        delete oldHead;

        pthread_mutex_lock(&mutex);
        length--;
        pthread_mutex_unlock(&mutex);
        return;
    }

    Node *oldHead = head;
    Node *newHead = head->prev;
    newHead->next = nullptr;
    head = newHead;
    delete oldHead;
    pthread_mutex_lock(&mutex);
    length--;
    pthread_mutex_unlock(&mutex);
}

void LinkedQueue::clear() {
    while (length) {
        pop();
    }
}

std::string *LinkedQueue::front() const {
    if (length != 0 && head != nullptr) {
        return &head->data;
    }
    return nullptr;
}

std::string *LinkedQueue::back() const {
    if (length != 0 && tail != nullptr) {
        return &tail->data;
    }
    return nullptr;
}

std::ostream &operator<<(std::ostream &os, const LinkedQueue &queue) {
    if (queue.size() == 0) {
        os << "[]";
        return os;
    }

    Node *headNode = queue.getHead();
    os << "[";

    for (int i = 0; i < queue.size(); ++i) {
        if (i == queue.size() - 1) {
            os << headNode->data << "]";
            continue;
        }
        os << headNode->data << ", ";
        headNode = headNode->prev;
    }
    return os;
}
