//
// Created by zhudanqi on 16-8-30.
//

#include <zdq_event_loop.h>
#include "zdq_tcp_client.h"

using namespace ZDQ;
using namespace std;

TcpClient * g_client;

void timeout()
{
    cout<<"time out "<<endl;
    g_client->stop();
}

int main()
{
    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 9981); // no such server
    TcpClient client(&loop, serverAddr, "TcpClient");
    g_client = &client;
    loop.runAfter(0.0, timeout);
    loop.runAfter(1.0, std::bind(&EventLoop::quit, &loop));
    client.connect();
    sleep(3);
    loop.loop();
    return 0;
}
