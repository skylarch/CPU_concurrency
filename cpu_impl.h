/**
 * cpu_impl.h
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */

#ifndef _CPU_IMPL_H
#define _CPU_IMPL_H

#include<queue>
#include "ucontext.h"
#include "cpu.h"
#include "thread_impl.h"

class cpu::impl {
public:
	// Keep track of thread id
    int num_thread;

    // Ready threads queue
    std::queue<thread::impl *> ready_queue;

    // Threads that finished running & out of scope
    std::queue<thread::impl *> finished_queue;

    // Current running thread
    thread::impl * current_thread;

    // Notify thread constructor if the thread is the first thread created
    // Control interrupt environment in thread constructor
    bool first_thread;

    static void switch_thread();
    static void timer_handler();
    static void delete_finished_queue();
};


#endif /* _CPU_IMPL_H */

