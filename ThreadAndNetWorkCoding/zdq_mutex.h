/*
 * 互斥量及互斥量资源管理类
 * 核心 ： 利用RAII管理互斥量
 * 作用 ： 利用互斥量，防止线程争用
 * 作者 ： zhu-dq
 */
#ifndef _ZDQ_MUTEX_H
#define _ZDQ_MUTEX_H
#include "zdq_noncopyable.h"
#include <pthread.h>
#include <assert.h>

namespace ZDQ{
class MutexLock:public ZDQ::NonCopyable{
    public:

        friend class MutexLockGuard;

        MutexLock():is_lock_(false)
        {
            pthread_mutex_init(&mutex_,NULL);
        }

        ~MutexLock()
        {
            assert(!is_lock_);
            pthread_mutex_destroy(&mutex_);
        }

        pthread_mutex_t * getMutexPtr()
        {
            return &mutex_;
        }

        bool isLocked()const
        {
            return is_lock_;
        }

    private:
        //防止手工调用
        void lock()
        {
            pthread_mutex_lock(&mutex_);
            is_lock_ = true;
        }

        void unlock()
        {
            is_lock_ = false;
            pthread_mutex_unlock(&mutex_);
        }

        pthread_mutex_t mutex_;
        bool is_lock_;
};
class MutexLockGuard:public ZDQ::NonCopyable{
    public:

        MutexLockGuard(MutexLock & mutex):mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:

        MutexLock & mutex_;
};
}
#endif
