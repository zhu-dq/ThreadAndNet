//
// Created by zhudanqi on 16-8-29.
//

#ifndef ZDQ_TCP_CLIENT_H
#define ZDQ_TCP_CLIENT_H

#include "zdq_noncopyable.h"
#include "zdq_tcp_connection.h"
#include "zdq_mutex.h"

namespace ZDQ{

    class EventLoop;
    class Connector;

    class TcpClient:public ZDQ::NonCopyable{

    public:
        typedef std::shared_ptr<Connector> ConnectorPtr;

        TcpClient(EventLoop* loop, const InetAddress& serverAddr,string name = "socket");
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        ZDQ::TcpConnection::TcpConnectionPtr getConnPtr()const
        {
            MutexLockGuard g(mutex_);
            return connection_;
        }

        EventLoop * getLoop()const{ return loop_;}

        bool IsRetry()const{ return  retry_;}

        void enableRetry(){retry_ = true;}

        void setConnCallback(const TcpConnection::ConnCallback & cb)
        {
            connCallback_ = cb;
        }

        void setMessCallback(const TcpConnection::MessageCallback & cb)
        {
            messCallback_ = cb;
        }

        void setWriteCompleteCallback(const TcpConnection::WriteCompleteCallback & cb)
        {
            writeCallback_ = cb;
        }



    private:
        void newConnection(int sockfd);
        void removeConnection(const ZDQ::TcpConnection::TcpConnectionPtr & conn);

        EventLoop * loop_;
        ConnectorPtr connector_;
        ZDQ::TcpConnection::ConnCallback connCallback_;
        ZDQ::TcpConnection::MessageCallback messCallback_;
        ZDQ::TcpConnection::WriteCompleteCallback writeCallback_;
        bool retry_;
        bool connected_;
        string name_;
        int nextConnId_;
        mutable MutexLock mutex_;
        ZDQ::TcpConnection::TcpConnectionPtr connection_;
    };
}


#endif //ZDQ_TCP_CLIENT_H
