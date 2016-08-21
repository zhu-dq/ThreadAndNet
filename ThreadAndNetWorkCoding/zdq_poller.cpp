//
// Created by zhudanqi on 16-8-7.
//

#include "zdq_poller.h"
#include <sys/poll.h>
#include <cassert>
#include "zdq_channel.h"
#include "zdq_event_loop.h"
ZDQ::Poller::Poller(EventLoop *loop) :ownerLoop_(loop)
{

}

ZDQ::Poller::~Poller()
{

}

void ZDQ::Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
    for(auto pfd = pollfds_.begin();pfd!=pollfds_.end()&&numEvents>0;++pfd)
    {
        if(pfd->revents>0)
        {
            --numEvents;
            auto ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            auto channel = ch->second;
            assert(channel->get_fd() == pfd->fd);
            channel->setREvent(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

ZDQ::Timestamp ZDQ::Poller::poll(int timeoutMs, ChannelList *activeChannels)
{
    //timeoutMs指的是毫秒 常量INFTIM指的是永远等待。详见UNP1:P144。numEvents:就绪描述符的个数
    int numEvents = ::poll(&pollfds_[0],pollfds_.size(),timeoutMs);//核心
    Timestamp now(Timestamp::now());
    if(numEvents>0)
        fillActiveChannels(numEvents,activeChannels);
    return now;
}
void ZDQ::Poller::updateChannel(Channel *c)
{
    assert(ownerLoop_->isInLoopThread());

    //channel的index记录自己在pollfds_数组中的位置
    if(c->getIndex()<0)//pollfds_ and channels 不存在
    {
        assert(channels_.find(c->get_fd())==channels_.end());

        struct pollfd pfd;
        pfd.fd = c->get_fd();
        pfd.events = static_cast<short >(c->getEvent());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size())-1;
        c->setIndex(idx);
        channels_[pfd.fd] = c;
    }
    else  //pollfds_ and channels 中已经存在
    {
        assert(channels_.find(c->get_fd())!=channels_.end());
        assert(channels_[c->get_fd()]==c);
        int idx = c->getIndex();
        assert(idx>=0 && idx<static_cast<int>(pollfds_.size()));
        auto & pfd = pollfds_[idx];
        pfd .events = static_cast<int>(c->getEvent());
        pfd.revents = 0;
        if(c->isNoneEvent())//用户没有设置需求,不关心任何事件
        {
            pfd.fd = -1;
        }
    }
}

void ZDQ::Poller::removeChannel(Channel *c)
{
    assert(ownerLoop_->isInLoopThread());
    int idx = c->getIndex();
    channels_.erase(c->get_fd());//1. 从channels中删除
    if(idx != pollfds_.size()-1)
    {
        std::swap(pollfds_[idx],pollfds_[pollfds_.size()-1]);
        channels_[pollfds_[idx].fd]->setIndex(idx);
    }
    pollfds_.pop_back();//2. 从pollfds中删除
}