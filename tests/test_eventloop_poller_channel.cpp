//
// Created by zhudanqi on 16-8-8.
//
#include "zdq_channel.h"
#include "zdq_poller.h"
#include "zdq_event_loop.h"
#include <sys/timerfd.h>
#include <iostream>
#include <string.h>

using namespace ZDQ;
using namespace std;

EventLoop * g_loop;

void timeout(Timestamp time)
{
    cout<<"timeout !"<<endl;
    g_loop->quit();
}

void test_case()
{
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.enableReading();//设置可读
    channel.setReadCallback(timeout);//设置可读后要做的事情

    struct itimerspec howlong;
    bzero(&howlong,sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd,0,&howlong,NULL);

    loop.loop();

    ::close(timerfd);
}

int main()
{
    test_case();
    return  0;
}
