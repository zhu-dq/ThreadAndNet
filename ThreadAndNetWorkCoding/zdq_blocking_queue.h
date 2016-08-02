/*
 * 无限容量阻塞队列
 * 特点：如果获取元素(take)但队列元素为空，就阻塞等待，直到有元素为止
 * 核心：对传统队列+条件变量（当然，条件变量需要锁，这里注意顺序，锁要比条件变量先声明）
 * 作者：zhu-dq
 */
#ifndef ZDQ_BLOCKING_QUEUE_H
#define ZDQ_BLOCKING_QUEUE_H
#include "zdq_condition.h"
#include <deque>

namespace ZDQ{
template<typename T>
class BlockingQueue:public ZDQ::NonCopyable{
    public:

        BlockingQueue():mutex_(),notEmpty_(mutex_),queue_(){} //注意顺序
        ~BlockingQueue(){}

        void put(const T& x)
        {
            ZDQ::MutexLockGuard g(mutex_);
            queue_.push_back(x);
            notEmpty_.notify();
        }

        T take()
        {
            ZDQ::MutexLockGuard g(mutex_);//必须
            while(queue_.empty())
            {
                notEmpty_.wait();
            }
            assert(!queue_.empty());
            T front(queue_.front());
            queue_.pop_front();
            return front;
        }

        size_t size()const
        {
            ZDQ::MutexLockGuard g(mutex_);
            return queue_.size();
        }

    private:

        mutable ZDQ::MutexLock mutex_;//注意声明顺序
        ZDQ::Condition notEmpty_;
        std::deque<T> queue_;
};
}
#endif
