#include "zdq_event_loop.h"
#include "zdq_channel.h"
#include "zdq_poller.h"
__thread ZDQ::EventLoop * t_loopInThisThread = 0; //__thread 保证单线程内的全局变量，其他线程不可访问
const int kPollTimeMs = 10000;
ZDQ::EventLoop::EventLoop():looping_(false),threadId_(ZDQ::Thread::getThreadId()),poller_(new Poller(this)),
                            quit_(false)
{
    std::cout<<"EventLoop created "<< this<<" int thread "<<threadId_;
    if(t_loopInThisThread)
        std::cout<<"eventloop has creaded int current thread "<<threadId_<<std::endl;
    else
        t_loopInThisThread = this;
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
    //wakeup()
}

