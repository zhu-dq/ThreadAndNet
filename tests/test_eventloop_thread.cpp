//
// Created by zhudanqi on 16-8-15.
//

#include "zdq_eventloop_thread.h"
#include "zdq_event_loop.h"
using namespace ZDQ;
EventLoop* g_loop;
void printTid0()
{
    printf("pid = %d, tid = %d\n", getpid(), Thread::getThreadId());
    printf("now %s\n", Timestamp::now().toString().c_str());
    g_loop->quit();
}
void printTid()
{
    printf("pid = %d, tid = %d\n", getpid(), Thread::getThreadId());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void dump()
{
    std::cout<< "dump loop"<<std::endl;
}
int main()
{
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;

        printf("main");
        loop.runAfter(2.0,printTid0);
        loop.loop();
        printf("main loop exits \n");
    }
    sleep(1);
    {
        std::cout<<"======================================="<<std::endl;
        EventLoopThread loopThread;
        EventLoop* loop = loopThread.startLoop();
        loop->runAfter(7.0,dump);
        loop->runEvery(2.0, printTid);
        sleep(100);
        std::cout<<"==========thread loop exits============="<<std::endl;
    }
    return 0;
}