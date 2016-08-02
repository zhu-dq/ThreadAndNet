/*
 * 线程池
 * 核心：两个数据结构：一个存线程，一个存任务
 *      对于线程队列：在start()里面，所有线程start();在stop()里面等待所有线程join
 *      对于任务队列：从队列取出任务(take() => task) ,执行就好(task())
 *              注意: 1.主要在考虑点在队列为空(等take)和满(等put)时阻塞等待问题
 *                    2.最后stop()里notifyAll时唤醒阻塞，所以要考虑特殊情况
 * 作者：zhu-dq
 */
#ifndef ZDQ_THREAD_POOL_H
#define ZDQ_THREAD_POOL_H
#include "zdq_thread.h"
#include "zdq_condition.h"
#include <deque>
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
namespace ZDQ{
class ThreadPool:public ZDQ::NonCopyable{
    public:

        typedef std::function<void ()> Task; 
        ThreadPool();
        ~ThreadPool();
        void start(size_t size_queue,size_t size_thread);
        void stop();
        void addTask(const Task &);
    private:
        void runInThread();
        Task take();
        size_t  size_queue_;//队列大小
        std::deque<Task> b_queue_;//任务队列
        std::vector<std::unique_ptr<ZDQ::Thread>> threads_;//线程集合
        //控制任务队列是否满
        mutable ZDQ::MutexLock mutex_;
        ZDQ::Condition cond_notFull_;
        ZDQ::Condition cond_notEmpty_;
        bool is_start_;
};
}
#endif
