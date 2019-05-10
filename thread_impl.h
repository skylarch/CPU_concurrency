/**
 * thread_impl.h
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#ifndef _THREAD_IMPL_H
#define _THREAD_IMPL_H

#include "thread.h"
#include "ucontext.h"
#include <queue>

class thread::impl {
public:
    ucontext_t *ucontext_ptr;

    int thread_id;

    // Show if the thread has finished running
    bool context_finished;

    // if the thread object is out of scope
    bool object_finished;

    // Queue of threads waiting for the thread to finish
    std::queue<thread::impl *> join_queue;

    static void func_wrapper(thread_startfunc_t, void *);
    void thread_helper(thread_startfunc_t, void *);
};

#endif /* _THREAD_IMPL_H */
