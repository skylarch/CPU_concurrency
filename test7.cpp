//exits while holding the lock

#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
cv cv1;

int child1_done = 0;		// global variable; shared between the two threads

void child1(void *a)
{
    char *message = (char *) a;
    mutex1.lock();
    cout << "child 1 called with message " << message << ", setting child1_done = 1" << endl;
    child1_done = 1;
    cv1.signal();
    cout << "child1 signaled" << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    cout << "release lock" << endl;
    mutex1.unlock();

    thread t1 ((thread_startfunc_t) child1, (void *) "child 1 message");
    
   
    mutex1.lock();
    cout << "mutex1 locked" << endl;
    while (!child1_done) {
        cout << "parent waiting for child 1 to run\n";
        cv1.wait(mutex1);
        
    }
    
    

    cout << "parent finishing" << endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  0);
}