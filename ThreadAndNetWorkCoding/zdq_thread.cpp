/*
 * 线程入门：
 * pthread_create : 创建线程
 * pthread_join : 等待线程终止
 * pthread_self : 获取自身线程ID
 * pthread_detach : 类似守护进程，不等待先车给终止，与join相对
 * pthread_exit : 终止一个线程
 */
#include "zdq_thread.h"
using namespace ZDQ;

Thread::Thread():pid_(0),is_start_(false),is_join_(false)
{

}

Thread::Thread(const ThreadFunc & func):func_(func),pid_(0),is_start_(false),is_join_(false)
{

}

Thread::~Thread()
{
    if(is_start_&&is_join_)
        pthread_detach(pid_);
}

void * Thread::startThread(void * self)
{
    Thread * t = static_cast<Thread *>(self);
    t->func_();
    return NULL;
}

void Thread::start()
{
    assert(!is_start_);
    is_start_ = true;
   if(pthread_create(&pid_,NULL,&startThread,this))
   {
       is_start_ = false;
       std::cout<<"pthread_create error " <<std::endl;      
   }
}

void Thread::join()
{
    assert(is_start_);
    assert(!is_join_);
    is_join_ = true;
    pthread_join(pid_,NULL);
}

pid_t Thread::getThreadId()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

bool ZDQ::Thread::IsStarted()
{
    return is_start_;
}
