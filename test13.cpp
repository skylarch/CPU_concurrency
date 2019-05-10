// test broadcast
#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
mutex mutex2;
cv cv1;

int count = 2;

void child1(void *a) {
    mutex1.lock();
    char *message = (char *) a;
    cout << "child 1 called with message " << message << endl;
    while (count--) {
        cout << "count > 0, child1 wait" << endl;
        cv1.wait(mutex1);
    }
    count = 5;
    cout << "child 1 wake up, set count=5" << endl;
    cv1.broadcast();
    cout << "child1 broadcast" << endl;
    cout << "child1 finishes" << endl;
    mutex1.unlock();
}

void child2(void *a) {
    mutex1.lock();
    char *message = (char *) a;
    cout << "child 2 called with message " << message << endl;
    while (count--) {
        cout << "count > 0, child2 wait" << endl;
        cv1.wait(mutex1);
    }
    count = 0;
    cout << "child 2 wake up, set count=0" << endl;
    cv1.broadcast();
    cout << "child2 broadcast" << endl;

    cout << "child2 waits" << endl;
    mutex2.lock();
    cout << "child2 mutex2.lock()" << endl;
    mutex1.unlock();
    cv1.wait(mutex2);
    cout << "child2 wakes up" << endl;
    cout << "child2 finishes" << endl;
}

void parent(void *a) {
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 ((thread_startfunc_t) child1, (void *) "CHILD 1");
    thread t2 ((thread_startfunc_t) child2, (void *) "CHILD 2");

    mutex1.lock();
    cout << "mutex1 locked" << endl;
    while (count--) {
        cout << "count > 0, parent wait" << endl;
        cv1.wait(mutex1);
    }

    count = 3;
    cout << "parent wake up, set count=3" << endl;
    cv1.broadcast();
    cout << "parent broadcast" << endl;

    cout << "parent finishes" << endl;
    mutex1.unlock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  1);
}