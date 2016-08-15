//
// Created by zhudanqi on 16-8-10.
//

#include "zdq_timer.h"
#include <string.h>
#include <sys/poll.h>
#include <iostream>
#include <unistd.h>//read
#include <sys/eventfd.h>
#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

ZDQ::Timer::Timer() : is_started_(false)
{
    timerfd_ = ::timerfd_create(CLOCK_REALTIME,0); //NODE1
    if(timerfd_ == -1)
        ERR_EXIT("Timer::timer()");
    bzero(&howlong_,sizeof howlong_);
}

ZDQ::Timer::~Timer()
{
    ::close(timerfd_);
}

void ZDQ::Timer::setTimer(int val, int interval)
{
    howlong_.it_value.tv_sec = val;//初始时间
    howlong_.it_interval.tv_sec = interval;//间隔时间
}

void ZDQ::Timer::setTimerCallback(const TimerCallback &cb)
{
    timerCallback_ = cb;
}

void ZDQ::Timer::runTimer()
{
    struct pollfd event[1];
    event[0].fd = timerfd_;
    event[0].events = POLLIN;
    char buf[1024];
    int nready;

    if(timerfd_settime(timerfd_,0,&howlong_,NULL)==-1)//NODE2
        ERR_EXIT("settime");

    is_started_ = true;

    while (is_started_)
    {
        nready = ::poll(event,1,10000);//10s
        if(nready == -1)
            ERR_EXIT("poll");
        else if (nready == 0)
            std::cout<<"timeout no event //等了10秒"<<std::endl;
        else
        {
            //这里必须read，否则poll被不停的触发
            if(::read(timerfd_,buf,sizeof buf)==-1)
                ERR_EXIT("read");
            timerCallback_();
        }
    }
}

void ZDQ::Timer::cannelTimer()
{
    bzero(&howlong_,sizeof howlong_);
    if(timerfd_settime(timerfd_,0,&howlong_,NULL)==-1)
        ERR_EXIT("settime");
    is_started_ = false;
}

int ZDQ::creatTimerFd()
{
    /*
    int timer_fd = ::timerfd_create(CLOCK_REALTIME,0);
    if(timer_fd == -1)
        ERR_EXIT("Timer::creatTimerFd");
    return timer_fd;
     */
    int evtfd = ::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
        ERR_EXIT("Timer::creadTimerFd");
    return evtfd;
}

void ZDQ::weakFd(int fd)
{
    uint64_t one = 1;
    ssize_t n = ::write(fd, &one, sizeof one);
    if (n != sizeof one) {
        std::cout << "EventLoop::wakeup() writes " << n << " bytes instead of 8"<<std::endl;
    }
}

void ZDQ::handleRead(int fd)
{
    uint64_t one = 1;
    ssize_t n = ::read(fd, &one, sizeof one);
    if (n != sizeof one)
    {
        std::cout << "EventLoop::handleRead() reads " << n << " bytes instead of 8"<<std::endl;
    }
}

struct timespec ZDQ::howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.getMicroSeconds()
                           - Timestamp::now().getMicroSeconds();
    if (microseconds < 100)
    {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(
            microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(
            (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void ZDQ::resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
        std::cout<< "timerfd_settime()"<<std::endl;
}
/*
 *==============NODE1=====================
 * int timerfd_create(int clockid, int flags);
 * 它是用来创建一个定时器描述符timerfd
 * 第一个参数：clockid指定时间类型，有两个值：
 * CLOCK_REALTIME :Systemwide realtime clock. 系统范围内的实时时钟
 * CLOCK_MONOTONIC:以固定的速率运行，从不进行调整和复位 ,它不受任何系统time-of-day时钟修改的影响
 * 第二个参数：flags可以是0或者O_CLOEXEC/O_NONBLOCK
 */

/*
 *============NODE2========================
 * int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
 * 作用：用来启动或关闭有fd指定的定时器
 * 参数：
 *      fd：timerfd，有timerfd_create函数返回
 *      fnew_value:指定新的超时时间，设定new_value.it_value非零则启动定时器，否则关闭定时器，
 *                  如果new_value.it_interval为0，则定时器只定时一次，即初始那次，否则之后每隔设定时间超时一次
 *      old_value：不为null，则返回定时器这次设置之前的超时时间
 *      flags：1代表设置的是绝对时间；为0代表相对时间。
 */

/*
 *==============NODE3========================
 * int timerfd_gettime(int fd, struct itimerspec *curr_value);
 * 此函数用于获得定时器距离下次超时还剩下的时间。如果调用时定时器已经到期，
 * 并且该定时器处于循环模式（设置超时时间时struct itimerspec::it_interval不为0），
 * 那么调用此函数之后定时器重新开始计时。
 */