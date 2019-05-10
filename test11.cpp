//Join

#include <iostream>
#include <cstdlib>
#include "thread.h"

using std::cout;
using std::endl;

mutex print;

int count = 4;

void child1(void *arg);
void child2(void *arg);
void parent(void *arg);

void child1(void *arg){
	thread th2(child2, (void *) "child2");
	while (count != 0){
		th2.join();
		cout << "th2 join\n";
		print.lock();
		cout << "child1 running" << endl;
		print.unlock();
	}
	print.lock();
	cout << "child1 call signal" << endl;
	print.unlock();

	print.lock();
	print.lock();
	cout << "CHILD1 finishes\n";
	print.unlock();
}

void child2(void *arg){
	while (count != 0){
		print.lock();
		cout << "child2 running" << endl;
		print.unlock();
		count--;
	}
	print.lock();
	cout << "child2 call signal" << endl;
	print.unlock();

	print.lock();
	cout << "CHILD2 finishes\n";
	print.unlock();
}

void parent(void *arg){
	print.lock();
	cout << "PARENT running\n";
	print.unlock();

	thread th1(child1, (void *) "child1");
	th1.join();

	print.lock();
	cout << "PARENT finishes\n";
	print.unlock();
}

int main(){
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 10, false, false, 1);
}