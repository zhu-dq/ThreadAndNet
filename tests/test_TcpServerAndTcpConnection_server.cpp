//
// Created by zhudanqi on 16-8-20.
//
#include "zdq_tcp_connection.h"
#include "zdq_tcp_server.h"
#include <iostream>
#include <string>
using namespace std;
using namespace ZDQ;

void onConnection(const ZDQ::TcpConnection::TcpConnectionPtr & conn)
{

    if(conn->connected())
    {
        cout<<"new Connection : "<<conn->name()
            <<" from "<< conn->peerAddress().getIpAsString()
            <<" : "<<conn->peerAddress().getPort()<<endl;
    } else{
        cout<<"Connection : "<<conn->name()<<" is down \n"<<endl;
    }

}

void onMessage(const ZDQ::TcpConnection::TcpConnectionPtr & conn,string mesg,ZDQ::Timestamp now)
{
    cout<<"received : "<<mesg<<" from "<<conn->name()<<endl;
}

int main()
{
    cout<<"main : pid : "<<getpid()<<endl;
    InetAddress listenAddr(9981);
    EventLoop loop;

    TcpServer server(&loop,listenAddr);
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.start();

    loop.loop();
    return 0;
}