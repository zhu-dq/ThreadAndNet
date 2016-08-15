/*
 * EcentLoopThread类
 * 作用：实现one loop per thread
 * 作者：zhu-dq
 */
#ifndef ZDQ_EVENTLOOP_THREAD_H
#define ZDQ_EVENTLOOP_THREAD_H

#include "zdq_noncopyable.h"
#include "zdq_thread.h"
#include "zdq_condition.h"
namespace ZDQ{
    class EventLoop;
    class EventLoopThread:public ZDQ::NonCopyable{
    public:
        EventLoopThread()
                :loop_(NULL),
                 thread_(std::bind(&EventLoopThread::creadLoop,this)),
                 mutex_(),
                 cond_(mutex_),
                 exiting_(false)
        {

        }

        ~EventLoopThread();
        EventLoop * startLoop();//开启一个线程，在此线程里创建一个loop,并返回loop指针，保证loop与线程一对一

    private:
        void creadLoop();//用于创建loop，执行loop.loop

        EventLoop * loop_;
        Thread thread_;
        MutexLock mutex_;
        Condition cond_;
        bool exiting_;
    };

}


#endif //THREADANDNET_ZDQ_EVENTLOOP_THREAD_H
