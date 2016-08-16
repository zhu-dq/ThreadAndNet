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
