//Deadlock

#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
mutex mutex2;

cv cv1;

int child_done = 0;		// global variable; shared between the two threads

void sam(void *a)
{   
    char *message = (char *) a;
    mutex2.lock();
    cout << "locked mutex2 " << message << endl;

    mutex1.lock();

    cout << "sam running" << message << endl;

    mutex1.unlock();
    mutex2.unlock();



}

void queen(void *a)
{
    char *message = (char *) a;
    mutex1.lock();
    cout << "queen called with message " << message << endl;
    cout << "mutex1 locked" << endl;
    cout << "yield to sam" << endl;
    thread::yield();

    mutex2.lock();
    
    cout << "running child1" <<endl;

    mutex2.unlock();

    mutex1.unlock();

}

void king(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    //create child1 thread
    thread t1 ((thread_startfunc_t) queen, (void *) "queen test");
    cout << "t1 created " << endl;
    thread t2 ((thread_startfunc_t) sam, (void *) "sam test");
    cout << "t2 created " <<endl;

    cout << "yield to queen " <<endl;
    thread::yield();
    
    mutex1.lock();
    //should start running child
    cout << "yield to child"<<endl;
    thread::yield();
    cout << "came back to parent" <<endl;
    while (!child_done) {
        cout << "parent waiting for child to run\n";
        cv1.wait(mutex1);
    }
    cout << "parent finishing" << endl;
    mutex1.unlock();
    cout << "mutex1 unlocked" << endl;
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) king, (void *)100, false, false, 1);
}
