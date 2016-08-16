//
// Created by zhudanqi on 16-8-16.
//

#ifndef ZDQ_ACCEPTOR_H
#define ZDQ_ACCEPTOR_H

#include "zdq_noncopyable.h"
#include "zdq_socket.h"
#include "zdq_inet_address.h"
#include "zdq_channel.h"
#include "zdq_event_loop.h"

namespace ZDQ{
    class Acceptor:public ZDQ::NonCopyable{
    public:
        typedef std::function<void (int sockfd,const InetAddress &)> CallBack;
        Acceptor(EventLoop * loop, const InetAddress & listenAddr);
        void listen();
        void setConnCallback(const CallBack & cb){callback_ = cb;}
        bool isListenning()const{ return listening_;}
    private:
        void handleRead();
        EventLoop * loop_;
        Socket socket_;
        Channel channel_;
        CallBack callback_;
        bool  listening_;

    };
}

#endif
