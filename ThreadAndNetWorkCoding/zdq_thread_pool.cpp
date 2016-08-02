#include "zdq_thread_pool.h"
using namespace ZDQ;

ThreadPool::ThreadPool():size_queue_(0),mutex_(),cond_notFull_(mutex_),cond_notEmpty_(mutex_),is_start_(false)
{

}

ThreadPool::~ThreadPool()
{
    if(is_start_)
    {
        this->stop();
    }
}

void ThreadPool::start(size_t size_queue,size_t size_thread)
{
    size_queue_ = size_queue;
    is_start_ = true;

    for(size_t i = 0 ; i < size_thread ; ++i)
    {
        //核心，线程启动后一直通过runInThread函数“监听”任务队列
        threads_.push_back(std::unique_ptr<Thread>(new Thread(std::bind(&ThreadPool::runInThread,this))));
    }

    for(auto & elem : threads_)
    {
        elem->start();
    }
}

void ThreadPool::stop()
{
    if(!is_start_)
        return ;

    {
        MutexLockGuard g(mutex_);
        is_start_ = false;
        cond_notEmpty_.notifyAll();//激活所有正在等待的任务
    }

    std::for_each(threads_.begin(),threads_.end(),std::bind(&ZDQ::Thread::join,std::placeholders::_1));

    while(b_queue_.size()>0)b_queue_.pop_front();//清空还没执行的任务

    threads_.clear();
}

void ThreadPool::addTask(const Task & t)
{
    MutexLockGuard g(mutex_);
    while(b_queue_.size()>=size_queue_)
    {
        cond_notFull_.wait();  //A
    }
    
    assert(b_queue_.size()<size_queue_);
    b_queue_.push_back(t);
    cond_notEmpty_.notify();  //B
}

ThreadPool::Task  ThreadPool::take()
{
    MutexLockGuard g(mutex_);
    while(b_queue_.empty()&&is_start_)
    {
        cond_notEmpty_.wait();   //B
    }
    Task t;
    if(!b_queue_.empty())
    {
        t = b_queue_.front();
        b_queue_.pop_front();
        cond_notFull_.notify(); //A
    }
    return t;
}

void ThreadPool::runInThread()
{
    while(is_start_)  //while很关键
    {
        Task run(this->take());
        if(run)
        {
            run();
        }
    }
}
