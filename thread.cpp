/**
 * thread.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#include "thread.h"
#include "thread_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "ucontext.h"
#include <stdexcept>

/**
 * Modify: ready_queue.
 * Effects:  Create thread object.
 */
thread::thread (thread_startfunc_t func, void * arg) {

    // If the user created thread, disable interrupt
    if (!cpu::self()->impl_ptr->first_thread) {
        assert_interrupts_enabled();
        cpu::interrupt_disable();
    }
    
    // Throw error if no enough memory
    try {
        this->impl_ptr = new impl;
        this->impl_ptr->ucontext_ptr = new ucontext_t;
        char *stack = new char [STACK_SIZE];
        this->impl_ptr->thread_id = cpu::self()->impl_ptr->num_thread++;
        this->impl_ptr->context_finished = false;
        this->impl_ptr->object_finished = false;
        this->impl_ptr->ucontext_ptr->uc_stack.ss_sp = stack;
        this->impl_ptr->ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
        this->impl_ptr->ucontext_ptr->uc_stack.ss_flags = 0;
        this->impl_ptr->ucontext_ptr->uc_link = nullptr;
    }
    catch (std::bad_alloc& alloc_error) {
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw alloc_error;
    }

    makecontext(this->impl_ptr->ucontext_ptr, (void (*)()) impl_ptr->func_wrapper, 2, func, arg);

    // Push the thread_impl object to ready_queue
    cpu::self()->impl_ptr->ready_queue.push(impl_ptr);

    if (!cpu::self()->impl_ptr->first_thread) {
        assert_interrupts_disabled();
        cpu::interrupt_enable();
    }
    else {
        cpu::self()->impl_ptr->first_thread = false;
    } 
}

/**
 * Modify: finished_queue, object_finished
 * Effects: 1.set current thread object_finished
 *          2.delete all threads on finished_queue
 *          3.push current thread to finished_queue if both context finished
 *          & thread object out of scope
 */
thread::~thread() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    this->impl_ptr->object_finished = true;

    cpu::self()->impl_ptr->delete_finished_queue();

    if (this->impl_ptr->object_finished && this->impl_ptr->context_finished) {
        cpu::self()->impl_ptr->finished_queue.push(this->impl_ptr);
    }
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: join_queue
 * Effects:  push current thread to join_queue if caller thread not finished
 */
void thread::join() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();

    if (!this->impl_ptr->context_finished) {
        impl_ptr->join_queue.push(cpu::self()->impl_ptr->current_thread);
        cpu::self()->impl_ptr->switch_thread();
        assert_interrupts_disabled();  
    }
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: ready_queue, current_thread.
 * Effects:  Change the current running thread to next ready thread and run
 */
void thread::yield() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();

    cpu::self()->impl_ptr->ready_queue.push(cpu::self()->impl_ptr->current_thread);
    cpu::self()->impl_ptr->switch_thread();
    
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}
