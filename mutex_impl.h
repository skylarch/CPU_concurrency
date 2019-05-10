/**
 * mutex_impl.h
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#ifndef _MUTEX_IMPL_H
#define _MUTEX_IMPL_H

#include "thread.h"
#include <queue>
#include "ucontext.h"
#include "thread_impl.h"

class mutex::impl {
private:
public:
	// Thread id of mutex owner
    int owner = -1;

    // Queue of threads blocked by this mutex
    std::queue<thread::impl *> block_queue;

    void unlock_helper();
};

#endif /* _MUTEX_IMPL_H */
