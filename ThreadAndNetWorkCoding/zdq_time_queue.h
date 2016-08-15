/*
 * TimeQueue类
 * 作用：同时管理多个定时函数
 * 核心：一个TimeQueue <--对应一个->Chnnel<--对应一个-->timerfd
 *      一个TimeQueue <--对应一个->set<时间函数信息类>//set按定时时间由低到长排列
 *      1)把这个最低时间赋值给timerfd，作为定时触发时间
 *      2)当这个时间到时，获取在set中所有满足这个时间的事件执行
 *      3)执行完相应事件后为要判断这个定时时间是不是需要循环定时
 *      4）是，则重新设置时间，再次加入的set序列。这时要重新判断哪个时间最短，针对性给timerfd赋值
 *      //=======================================================================
 *      考虑另一个问题：如果在event还在poll等待的时候，这时候新加入一个新的元素，怎么办？
 *      1）唤醒poll，怎么唤醒：往timerfd写东西
 *      2）在poll语句后面跟一个重新整理set的函数。。。考虑各种set timerfd相关问题
 *      //=======================================================================
 *      另一个设计：为了保证线程安全，把添加定时事件统一放到loop所在I/O线程去做
 *      1)把TimeQueue中添加定时事件函数利用function+bind封装成cb传给eventloop
 *      2)eventloop会判断当前线程是不是其所属线程
 *      3)是，则执行cb()
 *      4)不是，则选存到eventloop的vector<func>里,wakeup唤醒pool
 */
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
