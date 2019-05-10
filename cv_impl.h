/**
 * cv_impl.h
 *
 * EECS 482: Project 2, Winter 2019
 *
 * Jiajun Peng, Youwei Chen, Xuetong Sun
 * pjiajun, skylarch, xuetong
 *
 */
#ifndef _CV_IMPL_H
#define _CV_IMPL_H

#include "cv.h"
#include "thread.h"
#include <queue>
#include "ucontext.h"
#include "thread_impl.h"

class cv::impl {
public:

	// Queue of threads sleeping on the cv
    std::queue<thread::impl *> sleeping_queue;
};

#endif /* _CV_IMPL_H */
