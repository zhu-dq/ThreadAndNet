//
// Created by zhudanqi on 16-8-29.
//

#include "zdq_tcp_client.h"
#include "zdq_connector.h"
#include "zdq_event_loop.h"

namespace {
    void removeConnection(ZDQ::EventLoop* loop, const ZDQ::TcpConnection::TcpConnectionPtr& conn)
    {
        loop->appendFuncInLoop(std::bind(&ZDQ::TcpConnection::connectDestroyed, conn));
    }
}

ZDQ::TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr,string name)
        :loop_(loop),
         connector_(new Connector(loop,serverAddr)),
         retry_(false),
         connected_(true),
         name_(name),
         nextConnId_(1)
{
    connector_->setNewConnectionCallback(
            std::bind(&TcpClient::newConnection, this, std::placeholders::_1));
}

ZDQ::TcpClient::~TcpClient()
{
    ZDQ::TcpConnection::TcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard g(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn)
    {
        assert(loop_ == conn->getLoop());
        ZDQ::TcpConnection::CloseCallback cb = std::bind(&::removeConnection, loop_ ,std::placeholders::_1);
        loop_->runInLoop(
                std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique)
        {
            conn->forceClose();
        }
    }
    else
    {
        connector_->stop();
    }
}

void ZDQ::TcpClient::connect()
{
    connected_ = true;
    connector_->start();
}

void ZDQ::TcpClient::disconnect()
{
    connected_ = false;
    {
        MutexLockGuard g(mutex_);
        if(connection_)
            connection_->shutdown();
    }
}

void ZDQ::TcpClient::stop()
{
    connected_ = false;
    connector_->stop();
}

void ZDQ::TcpClient::newConnection(int sockfd)
{
    assert(loop_->isInLoopThread());
    InetAddress peerAddr(InetAddress::getPeerInetAddress(sockfd));
    InetAddress localAddr(InetAddress::getLocalInetAddress(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.getPort(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;
    TcpConnection::TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    conn->setConnectionCallback(connCallback_);
    conn->setMessageCallback(messCallback_);
    conn->setWriteCompleteCallback(writeCallback_);
    conn->setCloseCallback(
            std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void ZDQ::TcpClient::removeConnection(const ZDQ::TcpConnection::TcpConnectionPtr &conn)
{
    assert(loop_->isInLoopThread());
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->appendFuncInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connected_)
        connector_->restart();
}