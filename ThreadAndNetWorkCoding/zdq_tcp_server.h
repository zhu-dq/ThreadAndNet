/*
 *  TcpServer类
 *  功能：管理accept获取的TcpConnection
 *  作者:zhu-dq
 */

#ifndef ZDQ_TCP_SERVER_H
#define ZDQ_TCP_SERVER_H

#include "zdq_acceptor.h"
#include <string>
#include <memory>
#include <map>
#include "zdq_tcp_connection.h"

namespace ZDQ{
    class EventLoop;
    class TcpServer:public ZDQ::NonCopyable{
    public:
        TcpServer(EventLoop * loop,const InetAddress & listenAddr,string name = "socket");
        ~TcpServer(){}

        void start();
        void setConnectionCallback(const TcpConnection::ConnCallback & cb) {connCallback_=cb;}
        void setMessageCallback(const TcpConnection::MessageCallback & cb){messageCallback_=cb;}
        void removeConnection(const TcpConnection::TcpConnectionPtr& conn);
        void removeConnectionInLoop(const TcpConnection::TcpConnectionPtr& conn);

    private:
        void newConnection(int sockfd,const InetAddress & peerAddr);
        typedef std::map<std::string,TcpConnection::TcpConnectionPtr > ConnMap;
        EventLoop * loop_;
        unique_ptr<Acceptor> acceptor_;
        TcpConnection::ConnCallback connCallback_;
        TcpConnection::MessageCallback messageCallback_;
        const string name_;
        bool started_;
        int nextConnTd_;
        ConnMap conns_;
    };

}

#endif
