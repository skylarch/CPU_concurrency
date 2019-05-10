/**
 * cv.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */

#include "cv.h"
#include "cv_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "mutex.h"
#include "mutex_impl.h"
#include "ucontext.h"
#include "thread_impl.h"

/**
 * Effects:  Create cv object.
 */
cv::cv() {
    try {
        impl_ptr = new impl;
    }
    catch (std::bad_alloc& alloc_error) {
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw alloc_error;
    }
}

/**
 * Modifies: sleeping_queue, ready_queue.
 * Effects:  Wake up first thread on sleeping_queue, 
             push it to ready_queue.
 */
void cv::signal() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    
    if (!impl_ptr->sleeping_queue.empty()) {
        thread::impl * waked = impl_ptr->sleeping_queue.front();
        impl_ptr->sleeping_queue.pop();
        cpu::self()->impl_ptr->ready_queue.push(waked);
    }

    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: sleeping_queue, ready_queue.
 * Effects:  Wake up all threads on sleeping_queue, 
             push them to ready_queue.
 */
void cv::broadcast() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    
    while (!impl_ptr->sleeping_queue.empty()) {
        thread::impl * waked = impl_ptr->sleeping_queue.front();
        impl_ptr->sleeping_queue.pop();
        cpu::self()->impl_ptr->ready_queue.push(waked);
    }

    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: current_thread, sleeping_queue.
 * Effects:  Make current thread give up mutex, goes to sleep,
             and resume mutex ownership when waken up.
 */
void cv::wait(mutex& mutex) {
    assert_interrupts_enabled();
    cpu::interrupt_disable();

    //atomically unlock mutex, push to sleeping queue and switch thread
    mutex.impl_ptr->unlock_helper();

    assert_interrupts_disabled();

    impl_ptr->sleeping_queue.push(cpu::self()->impl_ptr->current_thread);
    cpu::impl::switch_thread();

    assert_interrupts_disabled();
    cpu::interrupt_enable();

    mutex.lock();

    assert_interrupts_enabled();
    
}

/**
 * Effects:  Delete cv object.
 */
cv::~cv(){
    delete impl_ptr;
    impl_ptr = nullptr;
}
