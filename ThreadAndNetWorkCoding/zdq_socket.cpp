//
// Created by zhudanqi on 16-8-16.
//

#include "zdq_socket.h"


int ZDQ::Socket::creatSockFd(const InetAddress &addr)
{
    /*
     //加SOCK_NONBLOCK 有问题，Socket::connect: Operation now in progress
    int socketFd = ::socket(
            addr.getSockAddrInet()->sin_family,
            SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
            IPPROTO_TCP);//参数信息，详见UNP1 P79
    if(socketFd < 0)
        ERR_EXIT("Socket::Socket(addr)");*/

    int socketFd = ::socket(addr.getSockAddrInet()->sin_family,
                            SOCK_STREAM, IPPROTO_TCP);//参数信息，详见UNP1 P79
    if(socketFd < 0)
        ERR_EXIT("Socket::Socket(addr)");
    return  socketFd;
}
void ZDQ::Socket::closeFd(int sockFd)
{
    if(::close(sockFd)<0)
        ERR_EXIT("Socket::closeFd");
}

int ZDQ::Socket::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        return errno;
    else
        return optval;
}

int ZDQ::Socket::createNonblockingOrDie(const InetAddress &addr)
{
    return ::socket(
            addr.getSockAddrInet()->sin_family,
            SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
            IPPROTO_TCP);//参数信息，详见UNP1 P79
}

int ZDQ::Socket::connectFdAndAddr(int sockfd,const InetAddress &addr)
{
    return ::connect(sockfd,(SA *)addr.getSockAddrInet(),sizeof(struct sockaddr_in));
}
