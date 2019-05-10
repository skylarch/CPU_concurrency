#include <iostream>
#include <cstdlib>
#include "thread.h"
#include <queue>

using std::cout;
using std::endl;

mutex m;
cv cv1;
std::queue<int> q;

void enqueue(void *a){
    intptr_t arg = (intptr_t) a;
    m.lock();
    q.push(arg);
    cout << "enqueueing " << q.front() << endl;
    cv1.signal();
    m.unlock();
}


void dequeue(void *a){
	m.lock();
    while (q.empty()){
       cv1.wait(m); 
    }
    cout << "dequeueing " << q.front() << endl;
    q.pop();
    m.unlock();
}
void parent(void *arg){
    m.lock();
	cout << "PARENT running\n";

	thread th1(dequeue, (void *) 0);
	thread th2(enqueue, (void *) 1);
	
    thread::yield();

    cout << "PARENT finishing\n";

    m.unlock();
}
int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 10, false, false, 0);
}