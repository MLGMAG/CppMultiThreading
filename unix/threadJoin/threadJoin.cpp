#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void sleep(int seconds) {
    cout << "Current thread go sleep for " << seconds << " seconds" << endl;
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void print(int n, const std::string &str) {
    cout << "Printing integer: " << n << endl;
    cout << "Printing string: " << str << endl;

    sleep(3);
}

int main() {
    thread t1(print, 666, "It's child thread!");

    cout << "Main thread: joining...." << endl;
    t1.join();
    cout << "Main thread: join completed!" << endl;

    return 0;
}
