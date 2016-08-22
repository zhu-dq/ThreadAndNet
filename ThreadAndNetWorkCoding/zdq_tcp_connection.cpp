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
    channel_->setReadCallback(bind(&ZDQ::TcpConnection::handleRead,this,std::placeholders::_1));
    //channel_->enableReading();
}


void ZDQ::TcpConnection::handleRead(Timestamp now)
{
    assert(loop_->isInLoopThread());
    /*
    char buf[65535];
    ssize_t n = ::read(channel_->get_fd(),buf, sizeof buf);
    string mesg;
    if(n>0)
    {
        for(int i = 0;i<n;++i)
            mesg.push_back(buf[i]);
        messageCallback_(shared_from_this(),mesg,ZDQ::Timestamp::now());
    }*/
    int savedErrno = 0;
    ssize_t n = inputBuf_.readFd(channel_->get_fd(), &savedErrno);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuf_, now);
    }
    else if(n == 0)
    {
        handleClose();
    } else
    {
        handleError();
    }

}

void ZDQ::TcpConnection::handleWrite()
{
    assert(loop_->isInLoopThread());
    if(channel_->isWriteing())
    {
        ssize_t n = ::write(channel_->get_fd(),outputBuf_.peek(),outputBuf_.readBuffSize());
        if(n > 0)
        {
            outputBuf_.retrieve(n);
            if(outputBuf_.readBuffSize() == 0)
            {
                channel_->disableWriteing();
                if(state_ == kDisconnecting)
                    shutdownInLoop();
            }
        }
    }
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

void ZDQ::TcpConnection::shutdown()
{
    if(state_ == kConnected)
    {
        setState(kDisconnecting);
        loop_->runInLoop(
                std::bind(&TcpConnection::shutdownInLoop,this));
    }
}

void ZDQ::TcpConnection::shutdownInLoop()
{
    assert(loop_->isInLoopThread());
    if(!channel_->isWriteing())
        socket_->shutdownWrite();
}

void ZDQ::TcpConnection::send(const std::string &message)
{
    if(state_ == kConnected)
    {
        if(loop_->isInLoopThread())
            sendInLoop(message);
        else
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,message));
    }
}

void ZDQ::TcpConnection::sendInLoop(const std::string &message)
{
    assert(loop_->isInLoopThread());
    ssize_t  nwrite = 0;
    if(!channel_->isWriteing() && outputBuf_.readBuffSize() == 0)
    {
        nwrite = ::write(channel_->get_fd(),message.data(),message.size());
        if(nwrite == -1)
            ERR_EXIT("ZDQ::TcpConnection::sendInLoop::write");
    }

    if(nwrite < message.size())
    {
        outputBuf_.append(message.data()+nwrite,message.size()-nwrite);
        if(!channel_->isWriteing())
            channel_->enableWriteing();
    }
}