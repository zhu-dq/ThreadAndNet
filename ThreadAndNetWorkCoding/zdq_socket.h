/*
 * 封装socket 基本调用函数
 * 知识点：见最下NODE
 * 作者：zhu-dq
 */
#ifndef ZDQ_SOCKET_H
#define ZDQ_SOCKET_H

#include "zdq_inet_address.h"
#include "zdq_noncopyable.h"
#include <unistd.h>  //close
#include <netinet/tcp.h>  //TCP_NODELAY
#include <iostream>
#include <fcntl.h>

namespace  ZDQ{

    class Socket : public ZDQ::NonCopyable{
    public:
        Socket(const InetAddress & addr):socket_fd_(Socket::creatSockFd(addr))
        {
        }
        explicit Socket(int sockfd):socket_fd_(sockfd){}

        ~Socket()
        {
            if(::close(socket_fd_)==-1)
                ERR_EXIT("close");
        }

        int getSocktfd()const
        {
            return socket_fd_;
        }

        void Bind(const InetAddress & addr)
        {
            if(::bind(socket_fd_,(SA*)addr.getSockAddrInet(),sizeof(struct sockaddr_in))==-1)
                ERR_EXIT("bind");
        }
        void Listen()
        {
            if(::listen(socket_fd_,SOMAXCONN)==-1)//SOMAXCONN defines the maximum number you're allowed to pass to listen()
                ERR_EXIT("listen");
        }

        int Accept(InetAddress * peeraddr)
        {
            int peerfd;
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            /*
            if(peerfd=::accept(socket_fd_,(SA *)&addr,&len) == -1)//这样写有问题，会出现服务器第一次启动，第一次客户端连接时获取不到服务器返回的信息
                ERR_EXIT("accept");*/

            peerfd=::accept(socket_fd_,(SA *)&addr,&len);
            if(peerfd>0)
            {
                peeraddr->setSockAddrInet(addr);
            } else{
                std::cout<<"connectfd error"<<std::endl;
                ::close(peerfd);
            }

            //fcntl(peerfd, F_SETFL, O_NONBLOCK); //非阻塞 io
            //perror("sccept : ");

            return  peerfd;
        }

        void shutdownWrite()//关闭连接的写这一半，其他详见UNP1:P136
        {
            if(::shutdown(socket_fd_,SHUT_WR)==-1)
                ERR_EXIT("shutdow");
        }

        //void Connect(InetAddress addr)
        int Connect(InetAddress addr)
        {
            /*
            struct sockaddr_in ADDR;
            ADDR.sin_family = AF_INET;
            ADDR.sin_port = htons(9981);
            inet_pton(AF_INET,"127.0.0.1",&ADDR.sin_addr);
            if(::connect(socket_fd_,(SA *)&ADDR,sizeof(ADDR))<0)*/

            /*if(::connect(socket_fd_,(SA *)addr.getSockAddrInet(),sizeof(struct sockaddr_in))<0)
                ERR_EXIT("Socket::connect");*/
            //::connect(socket_fd_,(SA *)addr.getSockAddrInet(),sizeof(struct sockaddr_in));

            return ::connect(socket_fd_,(SA *)addr.getSockAddrInet(),sizeof(struct sockaddr_in));
        }

        void sendMsg(std::string msg)
        {
            if((::send(socket_fd_,msg.c_str(),sizeof(msg.c_str()),0))<0)//send UNP1:P305
                ERR_EXIT("Socket::send");
        }
        std::string recvMsg()
        {
            char buf[2048];
            if((::recv(socket_fd_,buf,2048,0))<0)
                ERR_EXIT("Socket::recvMsg");
            //while(::recv(socket_fd_,buf,2048,0)<0);
            return std::string(buf);
        }
        /*
         *下面都是设置和获取套接字选项 内容太多，详见  UNP1:P151
         * setsockopt:设置套接字选项
         * getsockopt:获取套接字选项
         */

        void setReuseAddr(bool on)//允许重用本地地址
        {
            int opt_val = on ? 1:0;
            if((::setsockopt(socket_fd_,SOL_SOCKET,SO_REUSEADDR,&opt_val, static_cast<socklen_t>(sizeof(opt_val))))==-1)
                ERR_EXIT("setsockopt_ReuseAddr");
        }

        void setReusePort(bool on)//允许重用本地端口
        {
            int opt_val = on ? 1:0;
            if((::setsockopt(socket_fd_,SOL_SOCKET,SO_REUSEPORT,&opt_val, static_cast<socklen_t>(sizeof(opt_val))))==-1)
                ERR_EXIT("setsockopt_ReusePort");
        }

        void setKeepAlive(bool on)//周期性测试连接是否仍存活
        {
            int opt_val = on ? 1:0;
            if((::setsockopt(socket_fd_,SOL_SOCKET,SO_KEEPALIVE,&opt_val, static_cast<socklen_t>(sizeof(opt_val))))==-1)
                ERR_EXIT("setsockopt_ReusePort");
        }

        void setTcpNoDelay(bool on)//禁止Nagle算法
        {
            int opt_val = on ? 1:0;
            if((::setsockopt(socket_fd_,IPPROTO_TCP,TCP_NODELAY,&opt_val, static_cast<socklen_t>(sizeof(opt_val))))==-1)
                ERR_EXIT("setsockopt_TcpNoDelay");
        }

        static int creatSockFd(const InetAddress & addr);
        static int createNonblockingOrDie(const InetAddress &addr);
        static int  connectFdAndAddr(int sockfd,const InetAddress &addr);
        static int getSocketError(int sockfd);
        static void closeFd(int sockFd);
    private:
        const int socket_fd_;
    };
}

#endif
/*
 * ==============NODE1==================
 * socket : 创建套接字，指定通信类型
 * connect : TCP客户端用connect函数来建立与TCP服务的连接
 *          PS1:调用connect前不必非得调用bind函数
 *          PS2:调用connect会激发三次握手过程
 * bind : 把一个本地协议赋予一个套接字
 *      case1:如果未bind一个端口，当connect或listen时，内核就要为相应套接字选择一个临时端口
 *      case2:如果未bind一个IP地址：
 *              1）客户端：外出网络接口：客户源IP地址//TCP客户通常不把IP地址捆绑到它的套接字上
 *              2）服务端：客户发送目的IP：服务端源IP地址
 * listen : 仅有服务器端调用，把一个未连接套接字转换成一个被动套接字，指示内核应该接受指向该套接字的链接请求。
 * accept : 从已完成连接队列队头返回下一个已完成连接。
 * close : 1)该套接字标成已关闭
 *         2)TCP将尝试发送已排队等待发送到对端的任何数据。发送完毕后是正常的TCP连接终止序列
 * 其他详见UNP1:第四章
 * ==============NODE2 再聊listen==================
 *1)状态变化 ： 调用listen导致状态从CLOSED状态转换到LISTEN状态
 *2）内核为任一给定「监听套接字」维护两个队列：
 *              *)未完成队列（SYN已由客户发送到服务器，等待完成三次握手）,这些套接字状态：SYN_RCVD状态
 *              *)已完成队列（已完成三次握手的客户）,这些套接字状态：ESTABLISHED状态
 *              PS:对各种状态变化感兴趣，可看UNP1:P35
 *3)lisiten的backlog参数：曾被规定为这两个队列总和的最大值。。。。反正和两队列长度相关(常量SOMAXCONN表示允许listen数量的最大值)
 *4)UNP1:P85 图4-8
 * =================NODE3  close 与shutdown区别================================
 * 1）close把描述符的引用计数减1,仅在该计数变0时才关闭套接字。shutdown可以不管引用计数就激发TCP的正常连接终止序列
 * 2）close终止读写两个方面的数据传送。TCP是全双工，shutdown可以关闭读半边（SHUT_RD）/写半边（SHUT_WR）/两边都关闭（SHUT_RDWR）
 * =================NODE4 Nagle 算法===========================================
 * 问题：
 *      很多用户只发一个字节，这就产生一些41字节的分组：20字节的IP首部，20字节的TCP首部和1个字节的数据。
 *      在局域网上，这些小分组通常不会引起麻烦，因为局域网一般不会出现拥塞。但是在广域网上，这些小分组则会增加
 *      拥塞出现的可能。一种简单和好的方法就是采用Nagle算法。
 * Nagle算法：
 *      该算法要求一个TCP连接上最多只能有一个未被确认的未完成的小分组。在该分组确认到达之前不能发送其他的小分组。
 *      相反，TCP收集这些少量的分组，并在确认到来时以一个分组的方式发出去。该算法的优越之处在于它是自适应的：
 *      确认到达的越快，数据也就发送的越快。
 * 需要关闭Nagle算法的情况：
 *       有时候我们必须要关闭Nagle算法，特别是在一些对时延要求较高的交互式操作环境中，所有的小分组必须尽快发送出去。
 *       我们可以通过编程取消Nagle算法，利用TCP_NODELAY选项来关闭Nagle算法。
 * 作用：避免发送大量的小包
 * PS : Nagle算法并没有阻止发送小包，它只是阻止了发送大量的小包
 * nagle算法的初衷：避免发送大量的小包，防止小包泛滥于网络，理想情况下，对于一个TCP连接而言，
 * 网络上每次只能一个小包存在。它更多的是端到端意义上的优化。
 *
 * other : 详见 TCP/IP 1 : P203
 *
 * =================NODE5 应用层心跳=============================================
 * 心跳包的发送，通常有两种技术
 *
 * 方法1：应用层自己实现的心跳包
 *      由应用程序自己发送心跳包来检测连接是否正常，大致的方法是：服务器在一个 Timer事件中定时 向客户端发送一个
 *      短小精悍的数据包，然后启动一个低级别的线程，在该线程中不断检测客户端的回应，如果在一定时间内没有收到客户端的回应，
 *      即认为客户端已经掉线；同样，如果客户端在一定时间内没 有收到服务器的心跳包，则认为连接不可用。
 *
 * 方法2：TCP的KeepAlive保活机制
 *      因为要考虑到一个服务器通常会连接多个客户端，因此由用户在应用层自己实现心跳包，代码较多 且稍显复杂，
 *      而利用TCP／IP协议层为内置的KeepAlive功能来实现心跳功能则简单得多。 不论是服务端还是客户端，
 *      一方开启KeepAlive功能后，就会自动在规定时间内向对方发送心跳包， 而另一方在收到心跳包后就会自动回复，
 *      以告诉对方我仍然在线。 因为开启KeepAlive功能需要消耗额外的宽带和流量，
 *      所以TCP协议层默认并不开启KeepAlive功能，尽管这微不足道，但在按流量计费的环境下增加了费用，
 *      另一方面，KeepAlive设置不合理时可能会 因为短暂的网络波动而断开健康的TCP连接。
 *      并且，默认的KeepAlive超时需要7,200，000 MilliSeconds， 即2小时，探测次数为5次。
 *      对于很多服务端应用程序来说，2小时的空闲时间太长。
 *      因此，我们需要手工开启KeepAlive功能并设置合理的KeepAlive参数。
 *
 * [心跳包机制]
 *      跳包之所以叫心跳包是因为：它像心跳一样每隔固定时间发一次，以此来告诉服务器，这个客户端还活着。事实上这是为了保持长连接，
 *      至于这个包的内容，是没有什么特别规定的，不过一般都是很小的包，或者只包含包头的一个空包。在TCP的机制里面，
 *      本身是存在有心跳包的机制的，也就是TCP的选项：SO_KEEPALIVE。系统默认是设置的2小时的心跳频率。
 *      但是它检查不到机器断电、网线拔出、防火墙这些断线。而且逻辑层处理断线可能也不是那么好处理。
 *      一般，如果只是用于保活还是可以的。心跳包一般来说都是在逻辑层发送空的echo包来实现的。下一个定时器，
 *      在一定时间间隔下发送一个空包给客户端，然后客户端反馈一个同样的空包回来，
 *      服务器如果在一定时间内收不到客户端发送过来的反馈包，那就只有认定说掉线了。其实，要判定掉线，
 *      只需要send或者recv一下，如果结果为零，则为掉线。但是，在长连接下，有可能很长一段时间都没有数据往来。
 *      理论上说，这个连接是一直保持连接的，但是实际情况中，如果中间节点出现什么故障是难以知道的。更要命的是，
 *      有的节点（防火墙）会自动把一定时间之内没有数据交互的连接给断掉。在这个时候，就需要我们的心跳包了，
 *      用于维持长连接，保活。在获知了断线之后，服务器逻辑可能需要做一些事情，比如断线后的数据清理呀，
 *      重新连接呀……当然，这个自然是要由逻辑层根据需求去做了。总的来说，心跳包主要也就是用于长连接的保活和断线处理。
 *      一般的应用下，判定时间在30-40秒比较不错。如果实在要求高，那就在6-9秒。
 *
 * [心跳检测步骤]
 *      1.客户端每隔一个时间间隔发生一个探测包给服务器
 *      2.客户端发包时启动一个超时定时器
 *      3.服务器端接收到检测包，应该回应一个包
 *      4.如果客户机收到服务器的应答包，则说明服务器正常，删除超时定时器
 *      5.如果客户端的超时定时器超时，依然没有收到应答包，则说明服务器挂了
 *
 *      根据上面的介绍我们可以知道对端以一种非优雅的方式断开连接的时候，
 *      我们可以设置SO_KEEPALIVE属性使得我们在2小时以后发现对方的TCP连接是否依然存在。
 *
 * 作用：
 *      1. 维护长连接 ： 因为有的节点（防火墙）会自动把一定时间之内没有数据交互的连接给断掉。
 *      在这个时候，就需要我们的心跳包了，用于维持长连接，保活。
 *      2. 检测连接是否可用
 *
 */
