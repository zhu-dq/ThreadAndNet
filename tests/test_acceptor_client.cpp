//
// Created by zhudanqi on 16-8-16.
//
#include "zdq_socket.h"
#include "zdq_inet_address.h"
using namespace ZDQ;
int main()
{
    InetAddress addr("127.0.0.1",9981);
    Socket socket_(addr);
    socket_.Connect(addr);
    //socket_.sendMsg("nihao");
    std::cout<<socket_.recvMsg()<<std::endl;
    return 0;
}
