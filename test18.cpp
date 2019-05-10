
//test cv wait on multiple mutexes

#include <iostream>
#include <cstdlib>

#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
mutex mutex2;
cv cv1;

int child1_done = 0;		// global variable; shared between the two threads
int child2_done = 0;

void child1(void *a)
{
    char *message = (char *) a;
    mutex2.lock();
    cout << "child 1 called with message " << message << ", setting child1_done = 1" << endl;
    child1_done = 1;
    cv1.broadcast();
    cout << "child1 signaled" << endl;
    mutex2.unlock();
}
void child2(void *a)
{
    char *message = (char *) a;
    mutex2.lock();
    cout << "child 2 called with message " << message << ", setting child2_done = 1" << endl;
    cout << "mutex2 locked" << endl;
    while (!child1_done){
        cout << "child2 waiting for child 1 to run\n";
        cv1.wait(mutex2);
    }
    
    child2_done = 1;
    cv1.signal();
    cout << "child2 finished" << endl;
    mutex2.unlock();
}

void parent(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t2 ((thread_startfunc_t) child2, (void *) "child 2 message");
    thread t1 ((thread_startfunc_t) child1, (void *) "child 1 message");
    
    
   
    mutex1.lock();
    cout << "mutex1 lock" << endl;
    while (!child2_done) {
        cout << "parent waiting for child 2 to run\n";
        cv1.wait(mutex1);
        
    }
    
    cout << "parent finishing" << endl;
    mutex1.unlock();
    
   

    
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  0);
}
