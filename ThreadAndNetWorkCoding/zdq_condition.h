/*
 * 封装条件变量
 * 场景：当需要等待某个条件成立，应该使用条件变量
 * 核心：
 *      pthread_cond_wait : 等待。wait时解锁，被唤醒时加锁
 *      注意：
 *          在mutex已经上锁的时候才能调用wait
 *          把判断条件和wait放到while循环中
 *      //先修改bool相关变量，后使用signal/broadcast 
 *      pthread_cond_signal : 唤醒,表示资源可用
 *      pthread_cond_broadcast : 唤醒，表明状态变化
 *注意点：虚假唤醒，即使没有线程调用cond_signal,原先调用cond_wait的函数也可能返回
 *作者：zhu-dq
 */
#ifndef _CONDITION_H
#define _CONDITION_H
#include "zdq_noncopyable.h"
#include "zdq_mutex.h"
#include <pthread.h>
#include <assert.h>
namespace ZDQ{
    class Condition:public ZDQ::NonCopyable{
        public:

            Condition(ZDQ::MutexLock & mutex):mutex_(mutex)
            {
                pthread_cond_init(&cond_,NULL);
            }

            ~Condition()
            {
                pthread_cond_destroy(&cond_);
            }

            void wait()
            {
                assert(mutex_.isLocked());
                pthread_cond_wait(&cond_,mutex_.getMutexPtr());            
            }

            void notify()
            {
                pthread_cond_signal(&cond_);
            }

            void notifyAll()
            {
                pthread_cond_broadcast(&cond_);
            }

        private:
            pthread_cond_t  cond_;
            MutexLock & mutex_;
    };
}
#endif
