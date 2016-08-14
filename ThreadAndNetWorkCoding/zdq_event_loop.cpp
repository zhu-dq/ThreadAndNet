#include "zdq_event_loop.h"
#include "zdq_channel.h"
#include "zdq_poller.h"
#include "zdq_timer.h"
#include "zdq_time_queue.h"
__thread ZDQ::EventLoop * t_loopInThisThread = 0; //__thread 保证单线程内的全局变量，其他线程不可访问
const int kPollTimeMs = 10000;
ZDQ::EventLoop::EventLoop()
        :looping_(false),
         threadId_(ZDQ::Thread::getThreadId()),
         poller_(new Poller(this)),
         quit_(false),
         callingPendingFunctors_(false),
         wakeupFd_(ZDQ::creatTimerFd()),
         wakeupChannel_(this,wakeupFd_),
         timeQueue_(new TimeQueue(this))
{
    std::cout<<"EventLoop created "<< this<<" int thread "<<threadId_;
    if(t_loopInThisThread)
        std::cout<<"eventloop has creaded int current thread "<<threadId_<<std::endl;
    else
        t_loopInThisThread = this;

    wakeupChannel_.setReadCallback(std::bind(ZDQ::handleRead,wakeupFd_));
    wakeupChannel_.enableReading();
}

ZDQ::EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = NULL;
}

bool ZDQ::EventLoop::isInLoopThread()
{
    return threadId_ == ZDQ::Thread::getThreadId();
}

void ZDQ::EventLoop::loop()
{
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;
    quit_ = false;
    std::cout<<"event loop start looping"<<std::endl;
//    ::poll(NULL,0,5*1000);//什么都不做，等5秒
    while(!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs,&activeChannels_);//10000->10s
        for(auto & elem : activeChannels_)
            elem->handleEvent();
        doAppendFunctors();
    }
    std::cout<<"event loop stop looping"<<std::endl;
    looping_ = false;
}
void ZDQ::EventLoop::updateChannel(Channel *c)
{
    assert(c->ownerLoop()==this);
    assert(isInLoopThread());
    poller_->updateChannel(c);
}
void ZDQ::EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        ZDQ::weakFd(wakeupFd_);
}

void ZDQ::EventLoop::runInLoop(const Functor &cb)
{
    if(isInLoopThread())
        cb();
    else
        appendFuncInLoop(cb);
}

void ZDQ::EventLoop::appendFuncInLoop(const Functor &cb)
{
    {
        MutexLockGuard g(mutex_);
        appendFunctors_.push_back(cb);
    }
    //考虑两种情况。
    // 1.appendFuncInLoop所在的线程不是I/O线程
    // 2.如果I/O线程调用appendFuncInLoop，而此时正在调用pending functor.
    if(!isInLoopThread() || callingPendingFunctors_)
        ZDQ::weakFd(wakeupFd_);
}

void ZDQ::EventLoop::doAppendFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard g(mutex_);
        functors.swap(appendFunctors_);
    }

    for(auto & elem : functors)
        elem();
    callingPendingFunctors_ = false;
}

void ZDQ::EventLoop::runAt(const Timestamp &time, const Functor &cb)
{
    timeQueue_->addTimer(cb,time,0.0);
}

void ZDQ::EventLoop::runAfter(double delay, const Functor &cb)
{
    runAt(Timestamp::now().nowAfter(delay),cb);
}

void ZDQ::EventLoop::runEvery(double interval, const Functor &cb)
{
    timeQueue_->addTimer(cb,Timestamp::now().nowAfter(interval),interval);
}