/**
 * mutex.cpp
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#include "mutex.h"
#include "mutex_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <queue>
#include <stdexcept>
/**
 * Effects:  Create mutex object.
 */
mutex::mutex() {
    try {
        impl_ptr = new impl;
        impl_ptr->owner = -1;
    }
    catch (std::bad_alloc& alloc_error) {
        assert_interrupts_disabled();
        cpu::interrupt_enable();
        throw alloc_error;
    }
    
}

/**
 * Modifies: block_queue, mutex ownership.
 * Effects:  Acquire mutex if it is free,
             or goes to the block queue of the mutex.
 */
void mutex::lock() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();

    if (impl_ptr->owner != -1) {
        impl_ptr->block_queue.push(cpu::self()->impl_ptr->current_thread);
        cpu::impl::switch_thread();
    }
    else {
        impl_ptr->owner = cpu::self()->impl_ptr->current_thread->thread_id;
    }

    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Modifies: block_queue, ready_queue, mutex ownership.
 * Effects:  Unlock mutex.
 */
void mutex::unlock() {
    assert_interrupts_enabled();
    cpu::interrupt_disable();
        
    impl_ptr->unlock_helper();
        
    assert_interrupts_disabled();
    cpu::interrupt_enable();
}

/**
 * Effects:  Delete mutex object.
 */
mutex::~mutex(){
    delete impl_ptr;
    impl_ptr = nullptr;
}
