/*
 *事件循环类
 * 根据one loop per thread 思想 : 每个线程只能有一个EventLoop对象
 * 创建了EventLopop的线程是I/O线程，其主要功能是运行事件循环EventLoop:loop()
 * EventLoop对象的生命周期通常和其所属的线程一样长，它不必时heap对象
 * 核心：通关判断线程id来识别线程
 * 作者：zhu-dq
 */

#include "zdq_noncopyable.h"
#include "zdq_thread.h"
#include "zdq_mutex.h"
#include "zdq_channel.h"
#include "zdq_timer_info.h"
//#include <sys/poll.h>//::poll
#include <memory>
#include <vector>
#include <functional>

namespace ZDQ{
    class Channel;
    class Poller;
    class TimeQueue;
    class EventLoop : public ZDQ::NonCopyable
    {
        public:
            typedef std::vector<Channel*> ChannelList;
            typedef std::function<void () > Functor;
            EventLoop();
            ~EventLoop();
            void loop();//核心
            void updateChannel(Channel * c);//un_complite
            void removeChannel(Channel* c);
            bool isInLoopThread();
            void quit();
            //update 1
            void runInLoop(const Functor& cb);
            void appendFuncInLoop(const Functor& cb);
            void doAppendFunctors();
            //update 2
            void runAt(const Timestamp & time , const Functor& cb);
            void runAfter(double delay, const Functor& cb);
            void runEvery(double interval, const Functor& cb);
        private:
            bool looping_;
            const pid_t threadId_;//判断是否当前线程
            std::unique_ptr<Poller> poller_;//updateChannel
            ChannelList activeChannels_;
            bool quit_;
            //update for runInLoop
            MutexLock mutex_;
            std::vector<Functor> appendFunctors_;//因为暴露给了其他线程 所以要加锁
            bool  callingPendingFunctors_;

            //用于在有新的定时函数要加进来时，唤醒loop
            int wakeupFd_;
            Channel wakeupChannel_;

             std::unique_ptr<TimeQueue> timeQueue_;

    };
}

/*
 *============NODE1==================
 * getpid ： 取得process id，对于thread，就是取得线程对应进程的id；
 * gettid：取得线程id，如果是process，其实就等于getpid
 * ===========NODE2==================
 * 1)__thread是GCC内置的线程局部存储设施，存取效率可以和全局变量相比。
 * __thread变量每一个线程有一份独立实体，各个线程的值互不干扰。
 * 可以用来修饰那些带有全局性且值可能变，但是又不值得用全局变量保护的变量。
 * 2)__thread使用规则：只能修饰POD类型(类似整型指针的标量，不带自定义的构造、拷贝、赋值、析构的类型，
 * 二进制内容可以任意复制memset,memcpy,且内容可以复原)，
 * 不能修饰class类型，因为无法自动调用构造函数和析构函数，可以用于修饰全局变量，函数内的静态变量，
 * 不能修饰函数的局部变量或者class的普通成员变量，
 * 且__thread变量值只能初始化为编译器常量(值在编译器就可以确定const int i=5,运行期常量是运行初始化后不再改变
 */