//
// Created by zhudanqi on 16-8-7.
//
#include "zdq_channel.h"
#include <sys/poll.h>//temp
#include <iostream>//temp
#include "zdq_event_loop.h"

const  int ZDQ::Channel::KNoneEvent = 0;
const  int ZDQ::Channel::KReadEvent = POLLIN | POLLPRI;
const  int ZDQ::Channel::KWriteEvent = POLLOUT;

void ZDQ::Channel::handleEvent(Timestamp receiveTime)
{
    eventHandling_ = true;

    if(revents_ & POLLNVAL)
    {
        std::cout<<"Channel::handleEvent() POLLNVAL"<<std::endl;
    }

    if((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if(closeCallback_)
            closeCallback_();
    }

    if(revents_ & (POLLERR | POLLNVAL))
    {
        if(errorCallback_)
            errorCallback_();
    }

    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if(readCallback_)
            readCallback_(receiveTime);
    }

    if(revents_ & POLLOUT)
    {
        if(writeCallback_)
            writeCallback_();
    }

    eventHandling_ = false;
}

void ZDQ::Channel::update()
{
    loop_->updateChannel(this);
}

void ZDQ::Channel::remove()
{
    assert(isNoneEvent());
    loop_->removeChannel(this);
}