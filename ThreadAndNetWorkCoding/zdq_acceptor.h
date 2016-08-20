/*
 * Acceptor类
 * 作用：用于accept新TCP连接，并通过回调通知使用者.它是内部class，供TcpServer使用，生命周期由后者控制
 * 设计思路: socket channel eventloop
 *          1.socket 用于 bind listen accept
 *          2.channel负责事件分发，做socket与accept之间的桥梁. Accept（sockfd <--> channel） --update()-->eventloop
 *                                                            Accept响应<--channel做事件分发<--触发<--eventloop
 * 作者:zhu-dq
 */
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
