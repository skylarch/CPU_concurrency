#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
cv cv1;

int child_done = 0;		// global variable; shared between the two threads

void child(void *a)
{
    char *message = (char *) a;
    mutex1.lock();
    cout << "child called with message " << message << ", setting child_done = 1" << endl;
    child_done = 1;
    cv1.signal();
    cout << "after child signal" << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 ((thread_startfunc_t) child, (void *) "test message");
    cout << "constructed th1" << endl;
    mutex1.lock();
    while (!child_done) {
        cout << "parent waiting for child to run\n";
        cv1.wait(mutex1);
    }
    cout << "parent finishing" << endl;
    mutex1.unlock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  0);

}
