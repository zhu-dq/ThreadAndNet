//
// Created by zhudanqi on 16-8-20.
//

#include "zdq_tcp_connection.h"
#include "zdq_channel.h"
#include "zdq_event_loop.h"

ZDQ::TcpConnection::TcpConnection(EventLoop *loop, const string &name, int sockfd,
                                  const InetAddress &localAddr,
                                  const InetAddress &peerAddr):loop_(loop),
                                                               name_(name),
                                                               socket_(new Socket(sockfd)),
                                                               channel_(new Channel(loop,sockfd)),
                                                               localAddr_(localAddr),
                                                               peerAddr_(peerAddr),
                                                               state_(kConnecting)
{
    channel_->setReadCallback(bind(&ZDQ::TcpConnection::handleRead,this));
    //channel_->enableReading();
}


void ZDQ::TcpConnection::handleRead()
{
    char buf[65535];
    ssize_t n = ::read(channel_->get_fd(),buf, sizeof buf);
    string mesg;
    if(n>0)
    {
        for(int i = 0;i<n;++i)
            mesg.push_back(buf[i]);
        messageCallback_(shared_from_this(),mesg,ZDQ::Timestamp::now());
    }else if(n == 0)
    {
        handleClose();
    } else
    {
        handleError();
    }

}

void ZDQ::TcpConnection::handleWrite()
{

}

void ZDQ::TcpConnection::handleClose()
{
    assert(loop_->isInLoopThread());
    setState(kDisconnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void ZDQ::TcpConnection::handleError()
{
    int err = Socket::getSocketError(channel_->get_fd());
    std::cout<<"TcpConnection::handleEror(): err : "<<err<<std::endl;
}

void ZDQ::TcpConnection::connectEstablished()
{
    assert(loop_->isInLoopThread());
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->enableReading();
    connCallback_(shared_from_this());
}

void ZDQ::TcpConnection::connectDestroyed()
{
    assert(loop_->isInLoopThread());
    if(state_ == kConnected)
    {
        setState(kDisconnected);
        channel_->disableAll();
        connCallback_(shared_from_this());
    }
    loop_->removeChannel(channel_.get());
}