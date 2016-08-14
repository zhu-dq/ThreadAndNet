//
// Created by zhudanqi on 16-8-10.
//
#include <sys/timerfd.h>
#include <cassert>
#include "zdq_time_queue.h"
#include "zdq_timer_info.h"
#include "zdq_event_loop.h"
#include "zdq_timer.h"
#include <algorithm>
#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

int ZDQ::TimeQueue::creatTimerFd()
{
    int timer_fd = ::timerfd_create(CLOCK_REALTIME,0);
    if(timer_fd == -1)
        ERR_EXIT("Timer::timer()");
    return timer_fd;
}

ZDQ::TimeQueue::TimeQueue(EventLoop *loop)
        :loop_(loop),
         timerfd_(creatTimerFd()),
         timerfdChannel_(Channel(loop_,timerfd_)),
         timers_()
{
    timerfdChannel_.setReadCallback(std::bind(&TimeQueue::handleRead,this));
    timerfdChannel_.enableReading();
}

ZDQ::TimeQueue::~TimeQueue()
{
}

void ZDQ::TimeQueue::handleRead()
{
    assert(loop_->isInLoopThread());
    Timestamp now(Timestamp::now());
    std::vector<Entry>  expired = getAllExpiredTimers(now);
    for(auto & elem : expired)
        elem.second->run();
    reset(expired,now);
}

std::vector<ZDQ::TimeQueue::Entry> ZDQ::TimeQueue::getAllExpiredTimers(Timestamp now)
{
    std::vector<Entry > expired;
    Entry now_entry(now, reinterpret_cast<TimerInfo *>(UINTPTR_MAX));
    auto iter_end = timers_.lower_bound(now_entry);//lower_bound返回值一般是>= 给定val的最小指针（iterator）
    assert(iter_end == timers_.end() || now_entry.first < iter_end->first);
    std::copy(timers_.begin(),iter_end,std::back_inserter(expired));
    timers_.erase(timers_.begin(),iter_end);
    return  expired;
}

void ZDQ::TimeQueue::reset(std::vector<Entry> &expired, Timestamp now)
{
    for(auto & elem : expired)
    {
        if(elem.second->IsRepeat())
        {
            elem.second->restart(now);
            timers_.insert(elem);
        }
        else
        {
            delete elem.second;
            elem.second = NULL;
        }
    }
}

void ZDQ::TimeQueue::addTimer(const TimerCallback & cb, Timestamp when, double interval)
{
    TimerInfo * temp = new TimerInfo(cb,when,interval);
    loop_->runInLoop(
            std::bind(&TimeQueue::addTimerInloop,this,temp));
}

void ZDQ::TimeQueue::addTimerInloop(TimerInfo  * timer)
{
    assert(loop_->isInLoopThread());
    Timestamp when = timer->getExpiration();
    auto iter = timers_.begin();
    timers_.insert(Entry(timer->getExpiration(),timer));
    if(iter == timers_.end() || when < iter->first)
        ZDQ::resetTimerfd(timerfd_,when);
}