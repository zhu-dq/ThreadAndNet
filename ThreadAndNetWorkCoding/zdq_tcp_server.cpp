//
// Created by zhudanqi on 16-8-20.
//

#include "zdq_tcp_server.h"
ZDQ::TcpServer::TcpServer(EventLoop *loop,
                          const InetAddress &listenAddr,string name)
        :loop_(loop),
         acceptor_(new Acceptor(loop,listenAddr)),
         name_(name),
         started_(false),
         nextConnTd_(1)
{
    //acceptor对应socketfd,响应的是第一次建立连接的时候.此时会生成connfd
    acceptor_->setConnCallback(
            std::bind(&TcpServer::newConnection,this,
                      std::placeholders::_1,
                      std::placeholders::_2));//对应Accepter::handleRead里的connfd(_1)和peerAddr(_2)
    //acceptor_->handleRead();
    /*
       int connfd = socket_.Accept(&peeraddr);
     */
}

void ZDQ::TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    assert(loop_->isInLoopThread());
    char buf[32];
    snprintf(buf,sizeof buf,"#%d",nextConnTd_);
    ++nextConnTd_;
    std::string connName = name_ + buf;
    InetAddress localAddr(InetAddress::getLocalInetAddress(sockfd));
    //TcpConnection对应的是处理连接建立之后connfd是否可读(messageCallback_)及是否可写的情况，还有第一次建立连接要处理的事情(connCallback_)
    TcpConnection::TcpConnectionPtr  conn(new TcpConnection(loop_,connName,sockfd,localAddr,peerAddr));
    conns_[connName] = conn;
    conn->setConnectionCallback(connCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();
}

void ZDQ::TcpServer::start()
{
    assert(started_== false);
    started_ = true;
    loop_->runInLoop(
            std::bind(&Acceptor::listen,acceptor_.get()));
}