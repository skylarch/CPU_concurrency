//test switch thread
//cpu_init()
//thread::yield()
//mutex::lock()
//cv::wait()

#include <iostream>
#include "thread.h"

using std::cout;
using std::endl;

mutex mutex1;
mutex mutex2;
cv cv1;

int child_done = 0;		// global variable; shared between the two threads

void grandchild(void *a)
{   
    char *message = (char *) a;
    mutex2.lock();
    cout << "running grandchild " << message << endl;

    cout << "grandchild exiting " << message << endl;
    mutex2.unlock();



}

void child(void *a)
{
    char *message = (char *) a;
    mutex1.lock();
    cout << "child called with message " << message  << endl;
    thread t2 ((thread_startfunc_t) grandchild, (void *) "grandchild2 message");
    thread t3 ((thread_startfunc_t) grandchild, (void *) "grandchild3 message");
    t2.join();
    cout << "finish join grandchild 2" << endl;
    t3.join();
    cout << "finish join grandchild 3" << endl;
    child_done = 1;
    cout << child_done << endl;
    cv1.signal();
    cout << "cv1 signal" << endl;
    mutex1.unlock();
}

void parent(void *a)
{
    intptr_t arg = (intptr_t) a;
    mutex1.lock();
    cout << "parent called with arg " << arg << endl;
    mutex1.unlock();

    //yield shouldn't do anything here
    thread::yield();
    cout << "yield didn't do anything"<<endl;

    //create child thread
    thread t1 ((thread_startfunc_t) child, (void *) "test message");
    cout << "t1 created " << endl;
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
    cpu::boot(1, (thread_startfunc_t) parent, (void *)100, false, false,  0);
}
