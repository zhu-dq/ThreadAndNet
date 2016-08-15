//
// Created by zhudanqi on 16-8-7.
//

#include <sys/time.h>       //gettimeofday
#define __STDC_FORMAT_MACROS
#include <cinttypes>   //PRId64
#undef __STDC_FORMAT_MACROS
#include "zdq_timestamp.h"

ZDQ::Timestamp::Timestamp(double ms):microSeconds_(ms)
{

}

ZDQ::Timestamp::~Timestamp() {}

bool ZDQ::Timestamp::valid()
{
    return microSeconds_ > 0;
}

int64_t ZDQ::Timestamp::getMicroSeconds() const
{
    return microSeconds_;
}

double ZDQ::Timestamp::nowMicroSeconds()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);//核心 ，为什选这个，《Linux多线程服务端编程》:P241
    int64_t seconds = tv.tv_sec; //tv.tv_sec 秒
    return seconds*kMicroSecondsPerSecond+tv.tv_usec;//tv.tv_uesc  微秒
}

ZDQ::Timestamp ZDQ::Timestamp::now()
{
    return Timestamp(nowMicroSeconds());
}

ZDQ::Timestamp ZDQ::Timestamp::nowAfter(double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * kMicroSecondsPerSecond);
    return Timestamp(this->getMicroSeconds()+delta);
}

std::string ZDQ::Timestamp::toString()
{
    char buf[32] = {0};
    int second = microSeconds_/kMicroSecondsPerSecond;
    int micro_second = microSeconds_%kMicroSecondsPerSecond;
    snprintf(buf,sizeof(buf)-1,"%" PRId64 ".%06" PRId64 "",second,micro_second);
    return buf;
}

/*
 *============NODE2 PRId64===================
 * 头文件  ：   #include <cinttypes>
 *int64_t用来表示64位整数，在32位系统中是long long int，在64位系统中是long int，所以打印int64_t的格式化方法是:
 *      printf("%ld", value); // 64bit OS
 *      printf("%lld", value); // 32bit OS
 *跨平台方法：
 *      printf("%" PRId64 "\n", value);
 *注意点：
 *      这个是定义给c用的，C++要用它，就要定义一个__STDC_FORMAT_MACROS宏显示打开它
 */