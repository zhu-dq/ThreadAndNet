/*
 * EventLoopThreadPool类
 * 作用：类似线程池的一个EventLoop池，每个线程一个EventLoop。
 *      实现多线程TcpServer的关键步骤是在新建TcpConnection时从eventlooppool里挑选一个loop给TcpConnection
 * 核心：根据EventLoopThread类生成eventloop保证和线程一对一
 */

#ifndef ZDQ_EVENTLOOPTHREADPOOL_H
#define ZDQ_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <functional>
#include "zdq_noncopyable.h"

namespace ZDQ{

    class EventLoop;
    class EventLoopThread;

    class EventLoopThreadPool : ZDQ::NonCopyable{
    public:
        typedef std::function<void(EventLoop*)> ThreadInitCallback;
        EventLoopThreadPool(EventLoop * baseLoop);
        ~EventLoopThreadPool();
        void setThreadNum(int numThreads){numThreads_ = numThreads;}
        void start(const ThreadInitCallback& cb = ThreadInitCallback());
        EventLoop * getNextLoop();

    private:
        EventLoop * baseLoop_;
        bool started_;
        int numThreads_;
        int next_;
        std::vector<EventLoopThread *> threads_;
        std::vector<EventLoop *> loops_;
    };
}

#endif
