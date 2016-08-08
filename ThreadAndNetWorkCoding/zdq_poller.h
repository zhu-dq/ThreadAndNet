//
// Created by zhudanqi on 16-8-7.
//

#ifndef ZDQ_POLLER_H
#define ZDQ_POLLER_H

#include "zdq_noncopyable.h"
#include "zdq_timestamp.h"
#include <vector>
#include <map>

struct pollfd;

namespace ZDQ{
    class  Channel;
    class  EventLoop;
    class Poller : public ZDQ::NonCopyable{
    public:
        typedef std::vector<Channel*> ChannelList;

        Poller(EventLoop* loop);
        ~Poller();

        ZDQ::Timestamp poll(int timeoutMs,ChannelList* activeChannels);//核心
        void updateChannel(Channel * c);

    private:
        void fillActiveChannels(int numEvents,ChannelList * activeChannels)const;
        typedef std::vector<struct pollfd> PollList;
        typedef std::map<int,Channel*> ChannelMap;

        EventLoop * ownerLoop_;
        PollList pollfds_;
        ChannelMap channels_;//(pollfd->fd,channel *)
    };
}
#endif