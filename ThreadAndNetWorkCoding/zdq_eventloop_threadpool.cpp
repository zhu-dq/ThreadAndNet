//
// Created by zhudanqi on 16-8-28.
//

#include <cassert>
#include <cstdio>
#include "zdq_eventloop_threadpool.h"
#include "zdq_event_loop.h"
#include "zdq_eventloop_thread.h"

ZDQ::EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
        : baseLoop_(baseLoop),
          started_(false),
          numThreads_(0),
          next_(0)
{

}

ZDQ::EventLoopThreadPool::~EventLoopThreadPool()
{

}

void ZDQ::EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    assert(!started_);
    assert(baseLoop_->isInLoopThread());

    started_ = true;

    for (int i = 0; i < numThreads_; ++i)
    {
        EventLoopThread* t = new EventLoopThread;
        threads_.push_back(t);
        loops_.push_back(t->startLoop());
    }

    if (numThreads_ == 0 && cb)
        cb(baseLoop_);
}

ZDQ::EventLoop* ZDQ::EventLoopThreadPool::getNextLoop()
{
    assert(started_);
    assert(baseLoop_->isInLoopThread());

    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[next_++];
        if (next_ >= loops_.size())
            next_ = 0;
    }
    return loop;
}