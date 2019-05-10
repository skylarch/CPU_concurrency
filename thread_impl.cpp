/**
 * thread_impl.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#include "thread.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "ucontext.h"

/**
 * Modifies: join_queue, finished_queue.
 * Effects:  run user code, free threads on join_queue, clean up finished threads
 */

void thread::impl::func_wrapper (thread_startfunc_t func, void * arg) {
    assert_interrupts_disabled();
    cpu::interrupt_enable();
    (*func)(arg);

    assert_interrupts_enabled();
    cpu::interrupt_disable();

    cpu::self()->impl_ptr->current_thread->context_finished = true;

    // free the thread waiting on join queue
    while (!cpu::self()->impl_ptr->current_thread->join_queue.empty()) {
        thread::impl * temp = cpu::self()->impl_ptr->current_thread->join_queue.front();
        cpu::self()->impl_ptr->current_thread->join_queue.pop();
        cpu::self()->impl_ptr->ready_queue.push(temp);
    }

    cpu::self()->impl_ptr->delete_finished_queue();

    // push current thread into finished_queue if both context finished 
    // and thread object out of scope
    if (cpu::self()->impl_ptr->current_thread->object_finished && 
        cpu::self()->impl_ptr->current_thread->context_finished) {
        cpu::self()->impl_ptr->finished_queue.push(cpu::self()->impl_ptr->current_thread);
    }

    // start running the next thread context on ready queue. 
    // If ready queue is empty, call cpu suspend
    if (!cpu::self()->impl_ptr->ready_queue.empty()) {
        cpu::self()->impl_ptr->current_thread = cpu::self()->impl_ptr->ready_queue.front();
        cpu::self()->impl_ptr->ready_queue.pop();
        setcontext(cpu::self()->impl_ptr->current_thread->ucontext_ptr);
    }
    else {
        cpu::interrupt_enable_suspend();
    }
}