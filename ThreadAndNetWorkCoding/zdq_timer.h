/*
 * 定时器类
 * 核心:封装timerfd
 * 作者:zhu-dq
 */
#ifndef ZDQ_TIMER_H
#define ZDQ_TIMER_H

#include "zdq_timestamp.h"
#include "zdq_noncopyable.h"
#include <functional>
#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

namespace ZDQ{
    class Timer: public ZDQ::NonCopyable{
    public:
        typedef std::function<void () > TimerCallback;
        Timer();
        ~Timer();

        void setTimer(int,int);//(初始时间，间隔时间) //秒
        void setTimerCallback(const TimerCallback & cb);//timerfd可读时的响应事件
        void runTimer();
        void cannelTimer();
        int getTimerId()const{ return timerfd_;}
    private:
        int timerfd_;
        TimerCallback timerCallback_;
        struct itimerspec howlong_;
        bool is_started_;

    };

    int creatTimerFd();
    void weakFd(int fd);
    void handleRead(int fd);
    struct timespec howMuchTimeFromNow(Timestamp when);
    void resetTimerfd(int timerfd, Timestamp expiration);
}


#endif //ZDQ_TIMER_H
/*
 *===================NODE1===================
 */