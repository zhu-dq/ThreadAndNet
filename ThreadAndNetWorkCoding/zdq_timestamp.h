/*
 * 时间戳类
 * 核心：封装 gettimeofday / struct timeval (微秒)
 * 作者：zhu-dq
 */
#ifndef ZDQ_TIMESTAMP_H
#define ZDQ_TIMESTAMP_H

#include <sys/types.h>//int64_t
#include <string>
namespace ZDQ{
    class Timestamp{
    public:
        Timestamp(double ms = 0.0);
        ~Timestamp();

        bool valid();//判断是否有效

        int64_t getMicroSeconds()const;
        std::string  toString();

        static Timestamp now();
        static Timestamp nowAfter(double seconds);
        static double nowMicroSeconds();//核心
        static const int kMicroSecondsPerSecond = 1000*1000;
    private:
        int64_t microSeconds_;//微妙
    };
    bool operator <(Timestamp l, Timestamp r);
    bool operator ==(Timestamp l, Timestamp r);
}
#endif
/*
 *====================NODE1 linux时间函数=================
 * [计时函数]
 *         time/time_t  秒
 *         ftime/struct timeb   毫秒
 *         gettimeofday/struct timeval  微秒
 *         clock_gettime/struct timespec    纳秒
 *         为什么选gettimeofday?
 *              1. time精度太低;ftime已被抛弃;clock_gettime系统调用开销比gettimeofday大
 *              2. 在x86-64平台，gettimeofday不是系统调用，而是在用户态实现的，没有上下文切换和陷入内核的开销
 *              3. gettimeofday精度满足日常计时的需要
 */