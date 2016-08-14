//
// Created by zhudanqi on 16-8-10.
//

#ifndef ZDQ_TIME_QUEUE_H
#define ZDQ_TIME_QUEUE_H

#include "zdq_noncopyable.h"
#include "zdq_timestamp.h"
#include "zdq_channel.h"
#include "zdq_timer_info.h"
#include <set>
#include <vector>
#include <memory>
#include <functional>

namespace ZDQ{
    class EventLoop;
    class Channel;
    class Timestamp;
    class TimerId;
    class TimerInfo;
    class TimeQueue :public ZDQ::NonCopyable{
    public:
        typedef std::function<void () > TimerCallback;

        TimeQueue(EventLoop * loop);

        ~TimeQueue();

        static int creatTimerFd();

        void addTimer(const TimerCallback & cb,Timestamp when, double interval);//interval 间隔

        //void cannel(int timerId);

    private:
        typedef std::pair<Timestamp,TimerInfo* > Entry;
        typedef std::set<Entry> TimerList;

        void handleRead();//与timerfd_对应的回调函数
        std::vector<Entry> getAllExpiredTimers(Timestamp now);//所有当前定时时间到的事件
        void reset(std::vector<Entry>& expired, Timestamp now);
        //作为bc被传到loop,在loop所在I/O线程 执行：bc()
        void addTimerInloop(TimerInfo * timer );
        EventLoop * loop_; //轮寻监听 描述符
        const int timerfd_;//挂载到loop_上的描述符
        Channel timerfdChannel_;//与timerfd_的事件分发处理类
        TimerList timers_;//channel要处理的定时函数列表
    };

}


#endif
