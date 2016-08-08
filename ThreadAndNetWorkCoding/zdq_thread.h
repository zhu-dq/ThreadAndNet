/*
 * 线程类
 * 核心：封装pthread api
 * 作用：简化线程类
 * 作者：zhu-dq
 */
#ifndef _ZDQ_THREAD_H
#define _ZDQ_THREAD_H
#include "zdq_noncopyable.h"
#include <iostream>
#include <functional>  //function + bind 统一化线程函数
#include <pthread.h>
#include <assert.h>
#include <sys/syscall.h>//SYS_gettid
#include <unistd.h>//::syscall
namespace ZDQ{
    class Thread:public NonCopyable
    {
        public:

            typedef std::function<void ()> ThreadFunc;//线程函数类型

            Thread();

            explicit Thread(const ThreadFunc &);

            ~Thread();

            void start();

            void join();

            static pid_t getThreadId();

        private:

            static  void * startThread(void * obj);
            ThreadFunc func_;
            pthread_t pid_;
            bool is_start_;
            bool is_join_;
    };
}

#endif
