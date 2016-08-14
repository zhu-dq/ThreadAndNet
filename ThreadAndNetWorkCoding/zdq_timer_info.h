/*
 *定时器信息存储类
 * 作用：辅助timerqueue类
 * 作者：zhu-dq
 */
#ifndef ZDQ_TIMER_INFO_H
#define ZDQ_TIMER_INFO_H

#include <functional>
#include "zdq_noncopyable.h"
#include "zdq_timestamp.h"
#include "zdq_atomic.h"

namespace ZDQ{

    class TimerInfo;
    class Timestamp;

    /*
     *                     TimerId
     */

    class TimerId{
    public:
        TimerId()
                :timer_(NULL),
                 sequence_(0)
        {

        }

        TimerId(TimerInfo * timer,int64_t seq)
                :timer_(timer),
                 sequence_(seq)
        {

        }

        friend class TimerQueue;

    private:
        TimerInfo * timer_;//对应定时器信息
        int64_t sequence_;//对应序列号
    };


    /*
     *                  TimerInfo
     */

    class TimerInfo:public ZDQ::NonCopyable{
    public:
        typedef std::function<void () > TimerCallBack;
        TimerInfo(const TimerCallBack &cb, Timestamp when, double interval)
                :timerCallBack_(cb),
                 expiration_(when),
                 interval_(interval),
                 repeat_(interval>0.0),
                 sequence_(s_numCreated_.incrementAndGet())
        {

        }

        void run()const{timerCallBack_();}

        Timestamp getExpiration()const{ return expiration_;}

        bool IsRepeat()const { return  repeat_;}

        int64_t getSequence()const { return sequence_;}

        void restart(Timestamp now)
        {
            if(repeat_)
                expiration_ = now.nowAfter(interval_);
            else
                expiration_ = Timestamp(0);
        }

        static int64_t numCreated(){ return s_numCreated_.get();}

    private:
        TimerCallBack timerCallBack_;//定时器回调方法
        Timestamp expiration_;//下一次的超时时间
        const double interval_;				// 超时时间距离，假如是一次性定时器，该值为0
        const bool repeat_;					// 能否反复
        const int64_t sequence_;				// 定时器序号
        static AtomicInt64 s_numCreated_;		// 定时器计数，当前曾经创立的定时器数量
    };

}


#endif //THREADANDNET_ZDQ_TIMER_INFO_H
