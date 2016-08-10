/*
 * 定时器类
 * 核心:封装timerfd
 *	#include <sys/timerfd.h>
 *	timerfd_create
 *	timerfd_settime
 * 作者:zhu-dq
 */
#ifndef ZDQ_TIMER_H
#define ZDQ_TIMER_H

#include "zdq_timestamp.h"
#include "zdq_noncopyable.h"
#include <functional>
#include <sys/timerfd.h>
namespace ZDQ{
    class Timer: public ZDQ::NonCopyable{
    public:
        typedef std::function<void () > TimerCallback;
        Timer();
        ~Timer();

        void setTimer(int,int);//(初始时间（延时开始时间），间隔时间) //秒
        void setTimerCallback(const TimerCallback & cb);//timerfd可读时的响应事件
        void runTimer();
        void cannelTimer();
    private:
        int timerfd_;
        TimerCallback timerCallback_;
        struct itimerspec howlong_;
        bool is_started_;

    };

}


#endif //THREADANDNET_ZDQ_TIMER_H
/*
 *===================NODE1===================
 */
