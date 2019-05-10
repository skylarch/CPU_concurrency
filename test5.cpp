//DEADLOCK: lock multiple times; join with itself

#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;

mutex print;
mutex mlock;
cv cv_mlock;

int count = 4;

void child1(void *arg){
	((thread *)arg)->join();
	while (count != 0){
		print.lock();
		cout << "child1 running" << endl;
		print.unlock();
		count--;
	}
	print.lock();
	cout << "child1 call signal" << endl;
	cv_mlock.signal();
	print.unlock();

	print.lock();
	print.lock();
	cout << "CHILD1 finishes\n";
	print.unlock();
}

void child2(void *arg){
	mlock.lock();
	
    mlock.lock();
    
	while (count != 0){
		print.lock();
		cout << "child2 running" << endl;
		print.unlock();
		count--;
	}
	print.lock();
	cout << "child2 call signal" << endl;
	cv_mlock.signal();
	mlock.unlock();
	print.unlock();

	print.lock();
	cout << "CHILD2 finishes\n";
	print.unlock();
}

void parent(void *arg){
	print.lock();
	
	cout << "PARENT running\n";
	print.unlock();
	cout << "PARENT finishes unlock()" <<endl;

	thread th1(child1, (void *) &th1);
	thread th2(child2, (void *) "child2");
	mlock.lock();
	th1.join();

	print.lock();
	cout << "PARENT finishes\n";
	print.unlock();
}

int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 10, false, false, 1);
}