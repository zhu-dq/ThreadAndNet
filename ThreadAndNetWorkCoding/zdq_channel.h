/*
 * channel类
 * 功能：负责I/O事件分发
 * 特征：
 *      1）一个channel对象只属于一个IO线程
 *      2）一个channelaix自始至终只负责一个文件描述符(fd)
 *      3）channel会把不同的IO事件分发为不同的回调
 */
#ifndef ZDQ_CHANNEL_H
#define ZDQ_CHANNEL_H

#include <functional>
#include "zdq_noncopyable.h"

namespace ZDQ{
    class EventLoop;
    class Channel{
    public:
        typedef std::function<void ()> EventCallback;
        Channel(EventLoop* loop,int fd):loop_(loop),fd_(fd),events_(0),revents_(0),index_(-1){}

        void handleEvent();//核心，I/O事件分发

        //callback
        void setReadCallback(const EventCallback & func ){readCallback_ = func;}
        void setWriteCallback(const EventCallback & func ){writeCallback_= func;}
        void setErrorCallback(const EventCallback & func ){errorCallback_ = func;}

        //fd
        int get_fd()const { return fd_;}

        //event
        int getEvent()const{return events_;}
        bool isNoneEvent()const{return events_ == KNoneEvent;}
        void enableReading()
        {
            events_ |= KReadEvent; // 0 | 1 = 1 ; 1 | 1 = 1;
            update();
        }
        void enableWriteing(){events_ |= KWriteEvent;update();}
        void disableWriteing()
        {
            events_ &= ~KWriteEvent;// A & 0 = 0
            update();
        }
        void disableAll(){events_ = KNoneEvent;update();}

        //revent
        void setREvent(int revt){revents_ = revt;}

        //for poller
        int getIndex(){ return  index_;}
        void setIndex(int index){index_ = index;}

        EventLoop* ownerLoop()const { return loop_;}



    private:
        void update();
        static const int KNoneEvent;
        static const int KReadEvent;
        static const int KWriteEvent;

        EventLoop * loop_;
        const int fd_;
        int events_;//此channel关心的IO事件，由用户设置
        int revents_;//目前活动的事件，由eventloop/poller设置
        int index_;//used by poller

        EventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
    };

}

#endif //ZDQ_CHANNEL_H
