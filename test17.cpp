#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;

mutex printlock;

void child1(void *arg);
void child2(void *arg);
void parent(void *arg);

thread * thread_ptr;

void child1(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	thread_ptr->join();

	printlock.lock();
	cout << (char*)arg << " finishes\n";
	printlock.unlock();
}

void child2(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	thread::yield();

	printlock.lock();
	cout << (char*)arg << " finishes\n";
	printlock.unlock();
}

void parent(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	thread_ptr = new thread(child2, (void *) "th0");
	thread th1(child1, (void *) "th1");
	thread th2(child1, (void *) "th2");
	thread th3(child1, (void *) "th3");

	printlock.lock();
	cout << (char*)arg << " finish\n";
	printlock.unlock();
}

int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent", false, false, 1);
}