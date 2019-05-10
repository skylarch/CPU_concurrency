//SIGNALING WITHOUT HOLDING LOCK

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
	while (count != 0){
		print.lock();
		cout << (char *)arg << " running" << endl;
		print.unlock();
		count--;
	}
	print.lock();
	cout << (char *)arg << " call signal" << endl;
	cv_mlock.signal();
	print.unlock();

	print.lock();
	cout << (char *)arg << " finishes\n";
	print.unlock();
}

void child2(void *arg){
	mlock.lock();
	while (count != 0){
		print.lock();
		cout << (char *)arg << " running" << endl;
		print.unlock();
		count--;
	}
	print.lock();
	cout << (char *)arg << " call signal" << endl;
	cv_mlock.signal();
	mlock.unlock();
	print.unlock();

	print.lock();
	cout << (char *)arg << " finishes\n";
	print.unlock();
}

void parent(void *arg){
	print.lock();
	cout << "PARENT running\n";
	print.unlock();

	thread th1(child1, (void *) "child1");
	thread th2(child2, (void *) "child2");
	thread th3(child1, (void *) "child3");
	thread th4(child2, (void *) "child4");
	mlock.lock();
	if (count != 0){
		cv_mlock.wait(mlock);
	}

	print.lock();
	cout << "PARENT finishes\n";
	print.unlock();
}

int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 10, false, false, 1);
}