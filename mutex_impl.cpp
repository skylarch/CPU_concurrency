/**
 * mutex_impl.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 * mutex helper functions
 */
#include "mutex.h"
#include "mutex_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "thread_impl.h"
#include <stdexcept>

/**
 * Modifies: block_queue, ready_queue, mutex ownership.
 * Effects:  Release mutex.
 */
void mutex::impl::unlock_helper(){

    // Throw error if a thread tries to release mutex it does not hold
     if(owner != cpu::self()->impl_ptr->current_thread->thread_id){
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw std::runtime_error("Cannot unlock a mutex the thread is not holding");
        assert_interrupts_enabled();
        cpu::interrupt_disable();

    }

    owner = -1;
    // Make the first blocked thread the owner of the mutex;
    // Push it to ready queue;
    if (!block_queue.empty()) {
        thread::impl * temp = block_queue.front();
        block_queue.pop();
        cpu::self()->impl_ptr->ready_queue.push(temp);
        owner = temp->thread_id;
    }

}