//
// Created by zhudanqi on 16-8-16.
//


#include "zdq_acceptor.h"

using namespace ZDQ;
using namespace std;
void newConnection(int sockfd,const InetAddress & peerAddr)
{
    cout<<"accepted a new connection from "<<peerAddr.getIpAsString()<<" : "<<peerAddr.getPort()<<endl;
    ::write(sockfd,"how are you?\n",13);
    Socket::closeFd(sockfd);
}

int main()
{
    cout<<"main():pid : "<<getpid()<<endl;
    InetAddress addr(9981);
    EventLoop loop;

    std::cout<<"\n [server]       address : "<<addr.getIpAsString()<<" : "<<addr.getPort()<<std::endl;
    Acceptor acceptor(&loop,addr);
    acceptor.setConnCallback(newConnection);
    acceptor.listen();
    loop.loop();
    return 0;
}