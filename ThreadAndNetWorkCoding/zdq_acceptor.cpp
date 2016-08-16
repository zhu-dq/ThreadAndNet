//
// Created by zhudanqi on 16-8-16.
//

#include "zdq_acceptor.h"
ZDQ::Acceptor::Acceptor(EventLoop * loop, const InetAddress &listenAddr)
        : loop_(loop),
          socket_(listenAddr),
          channel_(loop,socket_.getSocktfd()),
          listening_(false)
{
    socket_.setReuseAddr(true);
    socket_.setReusePort(true);
    socket_.Bind(listenAddr);
    channel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}


void ZDQ::Acceptor::handleRead()
{
    assert(loop_->isInLoopThread());
    InetAddress peeraddr(0);
    int connfd = socket_.Accept(&peeraddr);
    assert(connfd>=0);
    if(callback_)
        callback_(connfd,peeraddr);
    else
    {
        if(::close(connfd)<0)
            ERR_EXIT("Acceptor::handleRead::close");
    }
}

void ZDQ::Acceptor::listen()
{
    assert(loop_->isInLoopThread());
    listening_ = true;
    socket_.Listen();
    channel_.enableReading();
}