//
// Created by zhudanqi on 16-8-14.
//

#include "zdq_eventloop_thread.h"
#include "zdq_event_loop.h"
using namespace ZDQ;
ZDQ::EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_!=NULL)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop * ZDQ::EventLoopThread::startLoop()
{
    assert(!thread_.IsStarted());
    thread_.start();//新线程执行loop
    {
        MutexLockGuard g(mutex_);
        while (loop_==NULL)
        {
            cond_.wait();
        }
    }
    return loop_;//原线程返回loop
}

void ZDQ::EventLoopThread::creadLoop()
{
    EventLoop loop;
    {
        MutexLockGuard g(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();//在新线程loop
    loop_ = NULL;
}