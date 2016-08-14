//
// Created by zhudanqi on 16-8-14.
//
#include "zdq_event_loop.h"
#include "zdq_thread.h"
#include <stdio.h>
#include <unistd.h>
#include <functional>

using namespace ZDQ;

int cnt = 0;
EventLoop* g_loop;

void printTid()
{
    printf("pid = %d, tid = %d\n", getpid(), Thread::getThreadId());
    printf("now %s\n", Timestamp::now().toString().c_str());
}

void print(const char* msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    if (++cnt == 20)
    {
        g_loop->quit();
    }
}


int main()
{
    printTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;

        print("main");
        loop.runAfter(1, std::bind(print, "once1"));
        loop.runAfter(1.5, std::bind(print, "once1.5"));
        loop.runAfter(2.5, std::bind(print, "once2.5"));
        loop.runAfter(3.5, std::bind(print, "once3.5"));
        loop.runAfter(4.5, std::bind(print, "once4.5"));
        loop.runEvery(2, std::bind(print, "every2"));
        loop.runEvery(3, std::bind(print, "every3"));
        loop.loop();
        print("main loop exits");
    }
}