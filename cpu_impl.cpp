/**
 * cpu.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 * cpu helper functions
 */

#include "cpu.h"
#include "cpu_impl.h"
#include "thread.h"
#include "thread_impl.h"
#include "ucontext.h"
#include <queue>

/**
 * Modifies: current_thread, ready_queue.
 * Effects:  switch to the next ready thread and run.
 */
void cpu::impl::switch_thread() {
    assert_interrupts_disabled();
    
    if (!cpu::self()->impl_ptr->ready_queue.empty()) {

        thread::impl * temp = cpu::self()->impl_ptr->current_thread;
        cpu::self()->impl_ptr->current_thread = cpu::self()->impl_ptr->ready_queue.front();
        cpu::self()->impl_ptr->ready_queue.pop();
        swapcontext(temp->ucontext_ptr, cpu::self()->impl_ptr->current_thread->ucontext_ptr);

    }

    // If no next ready thread, suspend cpu
    else {
        cpu::interrupt_enable_suspend();
    }
    
    assert_interrupts_disabled();
}

/**
 * Modifies: ready_queue.
 * Effects:  switch to the next ready thread and run.
 */
void cpu::impl::timer_handler() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    
    cpu::self()->impl_ptr->ready_queue.push(cpu::self()->impl_ptr->current_thread);
    switch_thread();
    
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: finished_queue.
 * Effects:  delete threads (object + context) on finish queue.
 */
void cpu::impl::delete_finished_queue() {
    assert_interrupts_disabled();
    while (!cpu::self()->impl_ptr->finished_queue.empty()) {
        thread::impl * temp = cpu::self()->impl_ptr->finished_queue.front();
        cpu::self()->impl_ptr->finished_queue.pop();

        // Delete stack
        delete[] (char *)temp->ucontext_ptr->uc_stack.ss_sp;
        temp->ucontext_ptr->uc_stack.ss_sp = nullptr;

        // Delete ucontext_ptr
        delete temp->ucontext_ptr;
        temp->ucontext_ptr = nullptr;

        // Delete impl_ptr
        delete temp;
        temp = nullptr;
    }
    assert_interrupts_disabled();
}