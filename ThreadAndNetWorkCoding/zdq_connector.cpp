//
// Created by zhudanqi on 16-8-29.
//

#include "zdq_connector.h"
#include "zdq_event_loop.h"
const int ZDQ::Connector::kMaxRetryDelayMs;

ZDQ::Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
        : loop_(loop),
          serverAddr_(serverAddr),
          connect_(false),
          state_(kDisconnected),
          retryDelayMs_(kInitRetryDelayMs)
{

}

ZDQ::Connector::~Connector()
{
    assert(!channel_);
}

void ZDQ::Connector::start()
{
    connect_ = true;
    loop_->runInLoop(std::bind(&Connector::startInLoop, this));
}

void ZDQ::Connector::startInLoop()
{
    assert(loop_->isInLoopThread());
    assert(state_ == kDisconnected);
    if(connect_)
        connect();
}

void ZDQ::Connector::restart()
{
    assert(loop_->isInLoopThread());
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();

}

void ZDQ::Connector::stop()
{
    connect_ = false;
    loop_->appendFuncInLoop(std::bind(&Connector::stopInLoop, this));
}

void ZDQ::Connector::stopInLoop()
{
    assert(loop_->isInLoopThread());
    if(state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void ZDQ::Connector::connect()
{
    int sockfd = Socket::createNonblockingOrDie(serverAddr_);
    int ret = Socket::connectFdAndAddr(sockfd,serverAddr_);
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            Socket::closeFd(sockfd);
            break;

        default:
            Socket::closeFd(sockfd);
            break;
    }

}

void ZDQ::Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(
            std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(
            std::bind(&Connector::handleError, this));
    channel_->enableWriteing();
}

void ZDQ::Connector::handleWrite()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        int err = Socket::getSocketError(sockfd);
        if (err)
        {
            retry(sockfd);
        }
        else if (InetAddress::isSelfConnect(sockfd))//处理自连接
        {
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
                newConnectionCallback_(sockfd);
            else
                Socket::closeFd(sockfd);
        }
    }
    else
    {
        assert(state_ == kDisconnected);
    }
}

void ZDQ::Connector::handleError()
{
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }

}

int ZDQ::Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->get_fd();
    loop_->appendFuncInLoop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void ZDQ::Connector::resetChannel()
{
    channel_.reset();
}

void ZDQ::Connector::retry(int sockfd)
{
    Socket::closeFd(sockfd);
    setState(kDisconnected);
    if (connect_)
    {
        loop_->runAfter(retryDelayMs_/1000.0,
                        std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    }
}