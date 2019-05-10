/**
 * cpu.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 * cpu_init
 */

#include "cpu.h"
#include "cpu_impl.h"
#include "thread.h"
#include "ucontext.h"
#include "thread_impl.h"
#include <stdio.h>

/**
 * Effects:  Initialize first thread.
 */
void cpu::init(thread_startfunc_t func, void * ptr) {
    assert_interrupts_disabled();

    // Initialize impl_ptr of cpu object and interrupt handler
    try {
        impl_ptr = new impl;
        impl_ptr->num_thread = 0;
        impl_ptr->first_thread = true;
        interrupt_vector_table[TIMER] = impl_ptr->timer_handler;
    }
    catch (std::bad_alloc& alloc_error) {
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw alloc_error;
    }
    
    // Create a thread that executes func(arg) if func not nullptr
    if (func) {
        assert_interrupts_disabled();

        thread th1(func, ptr);

        assert_interrupts_disabled();
        //set current thread to newly created thread
        impl_ptr->current_thread = th1.impl_ptr;
        
        // Pop the current running thread off the ready_queue
        if(!impl_ptr->ready_queue.empty()){
            impl_ptr->ready_queue.pop();
        }
        //start current thread
        setcontext(impl_ptr->current_thread->ucontext_ptr);
    }
    
}
