//
// Created by zhudanqi on 16-8-29.
//

#include <iostream>
#include "zdq_event_loop.h"
#include "zdq_connector.h"

using namespace ZDQ;
using namespace std;

EventLoop * g_loop;

void connectCallback(int sockfd)
{
    cout<<"connect."<<endl;
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    InetAddress addr("127.0.0.1",9981);
    Connector conn(&loop,addr);
    conn.setNewConnectionCallback(connectCallback);
    conn.start();

    loop.loop();

    return 0;
}
