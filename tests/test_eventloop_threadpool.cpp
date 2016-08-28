//
// Created by zhudanqi on 16-8-28.
//
#include <iostream>
#include "zdq_eventloop_threadpool.h"
#include "zdq_thread.h"
#include "zdq_event_loop.h"

using namespace std;
using namespace ZDQ;

void print(EventLoop* p = NULL)
{
    printf("main(): pid = %d, tid = %d, loop = %p\n",
           getpid(), Thread::getThreadId(), p);
}

void init(EventLoop* p)
{
    printf("init(): pid = %d, tid = %d, loop = %p\n",
           getpid(), Thread::getThreadId(), p);
}

int main()
{
    print();

    EventLoop loop;
    loop.runAfter(11, bind(&EventLoop::quit, &loop));

    {
        printf("Single thread %p:\n", &loop);
        EventLoopThreadPool model(&loop);
        model.setThreadNum(0);
        model.start(init);
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
    }

    {
        printf("Another thread:\n");
        EventLoopThreadPool model(&loop);
        model.setThreadNum(1);
        model.start(init);
        EventLoop* nextLoop = model.getNextLoop();
        nextLoop->runAfter(2,bind(print, nextLoop));
        assert(nextLoop != &loop);
        assert(nextLoop == model.getNextLoop());
        assert(nextLoop == model.getNextLoop());
        ::sleep(3);
    }

    {
        printf("Three threads:\n");
        EventLoopThreadPool model(&loop);
        model.setThreadNum(3);
        model.start(init);
        EventLoop* nextLoop = model.getNextLoop();
        nextLoop->runInLoop(std::bind(print, nextLoop));
        assert(nextLoop != &loop);
        assert(nextLoop != model.getNextLoop());
        assert(nextLoop != model.getNextLoop());
        assert(nextLoop == model.getNextLoop());
    }

    loop.loop();
}
