//unlock a mutex it's not holding
//should throw runtime error

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
    cout << child1_done <<endl;
    cv1.signal();
    cout << "cv1 signaled" << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    thread t1 ((thread_startfunc_t) child1, (void *) "child 1 message");
    cout << "created t1" << endl;
   
    mutex1.lock();
    while (!child1_done) {
        cout << "parent waiting for child 1 to run\n";
        cv1.wait(mutex1);
        
    }
    
    mutex1.unlock();
    cout << "mutex1 unlock" << endl;
    try{
    mutex1.unlock();
    cout << "mutex1 unlock" << endl;
    }
    catch(std::runtime_error& e){
        cout << e.what();
    }

    cout << "parent finishing" << endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100,false, false,  0);
}