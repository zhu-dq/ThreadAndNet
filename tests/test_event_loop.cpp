//
// Created by zhudanqi on 16-8-7.
//
#include <sys/timerfd.h>
#include <string.h>
#include "zdq_channel.h"
#include "zdq_event_loop.h"
#include <functional>
using namespace ZDQ;
ZDQ::EventLoop * g_loop;
void timeout(EventLoop * loop)
{
    std::cout<<"timeout !"<<std::endl;
    loop->quit();
}
void threadFunc()
{
    std::cout<<"进程ID : "<<getpid()<<"线程ID : "<<ZDQ::Thread::getThreadId()<<std::endl;
    ZDQ::EventLoop loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.enableReading();//设置可读
    channel.setReadCallback(std::bind(timeout,&loop));//设置可读后要做的事情

    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd,0,&howlong,NULL);
    loop.loop();
    ::close(timerfd);
}

void threadFun1()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(g_loop,timerfd);
    channel.setReadCallback(std::bind(timeout,g_loop));
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd,0,&howlong,NULL);
    g_loop->loop();
    ::close(timerfd);
}

void test_case1()
{
    std::cout<<"进程ID : "<<getpid()<<"线程ID : "<<ZDQ::Thread::getThreadId()<<std::endl;
    ZDQ::EventLoop loop;
    ZDQ::Thread t(threadFunc);
    t.start();
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.setReadCallback(std::bind(timeout,&loop));
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd,0,&howlong,NULL);
    loop.loop();
    ::close(timerfd);
    pthread_exit(NULL);
}

void test_case2()
{
    ZDQ::EventLoop loop;
    g_loop = &loop;
    ZDQ::Thread t(threadFun1);
    t.start();
    t.join();
}
int main()
{
    test_case1();//ok
    //test_case2();//ok
    return 0;
}
