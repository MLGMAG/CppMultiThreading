#include <iostream>
#include "linkedQueue.h"

using namespace std;

int main() {
    LinkedQueue linkedQueue;

    linkedQueue.push("aa");
    linkedQueue.push("b");
    linkedQueue.push("c");

    cout << linkedQueue << endl;

    while (linkedQueue.size()) {
        if (linkedQueue.front() == nullptr) {
            continue;
        }

        cout << "Current front: " << *linkedQueue.front() << endl;
        cout << "Current back: " << *linkedQueue.back() << endl;

        linkedQueue.pop();
        cout << endl << "POP" << endl;
    }

    cout << "Is front null: " << (linkedQueue.front() == nullptr ? "true" : "false") << endl;
    cout << "Is back null: " << (linkedQueue.back() == nullptr ? "true" : "false") << endl;

    linkedQueue.pop();

    return 0;
}
