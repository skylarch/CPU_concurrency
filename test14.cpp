// circular join
#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;
mutex mutex1;
cv cv1;

void child(void *a) {
    mutex1.lock();
    cout << "child acquire lock" << endl;

    try {
        mutex1.unlock();
        cout << "child successfully unlock" << endl;
    }
    catch (std::runtime_error&) {
        cout << "error: child has lock, but cannot successfully unlock" << endl;
    }

    try {
        mutex1.unlock();
        cout << "child try to acquire lock that it does not own" << endl;
    }
    catch (std::runtime_error&) {
        cout << "catch error: child try to unlock a lock it does not own" << endl;
    }

    try {
        cv1.wait(mutex1);
        cout << "child try to wait on cv (but does not have lock)" << endl;
    }
    catch (std::runtime_error&) {
        cout << "catch error: child try to wait on cv (but does not have lock)" << endl;
    }
    cout << "child finishes" << endl;
}

void parent(void *a) {
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 ((thread_startfunc_t) child, (void *) "child");
    cout << "constructed th1" << endl;
    mutex1.lock();
    thread::yield();
    mutex1.unlock();

    cout << "parent finishes" << endl;
}


int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  1);

}