#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;

mutex mlock;
mutex printlock;

void child1(void *arg);
void child2(void *arg);
void child3(void *arg);
void parent(void *arg);

void child1(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	mlock.lock();
	thread th3(child2, (void *) "th3");

	printlock.lock();
	cout << (char*)arg << " unlocking\n";
	printlock.unlock();

	mlock.unlock();

	printlock.lock();
	cout << (char*)arg << " finishes\n";
	printlock.unlock();
}

void child2(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	thread th4(child3, (void *) "th4");
	thread::yield();

	printlock.lock();
	cout << (char*)arg << " finishes\n";
	printlock.unlock();
}

void child3(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	mlock.lock();

	printlock.lock();
	cout << (char*)arg << " unlocking\n";
	printlock.unlock();
	
	mlock.unlock();

	printlock.lock();
	cout << (char*)arg << " finishes\n";
	printlock.unlock();
}

void parent(void *arg){
	printlock.lock();
	cout << (char*)arg << " running\n";
	printlock.unlock();

	mlock.lock();
	thread th1(child1, (void *) "th1");
	thread th2(child2, (void *) "th2");

	printlock.lock();
	cout << (char*)arg << " yielding\n";
	printlock.unlock();

	thread::yield();

	printlock.lock();
	cout << (char*)arg << " unlocking\n";
	printlock.unlock();

	mlock.unlock();
	thread::yield();

	printlock.lock();
	cout << (char*)arg << " finish\n";
	printlock.unlock();
}

int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent", false, false, 1);
}