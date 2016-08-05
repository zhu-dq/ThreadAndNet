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

namespace  ZDQ{

    class Socket : public ZDQ::NonCopyable{
    public:
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
            if(::bind(socket_fd_,(SA*)addr.getSockAddrInet(),sizeof(SA))==-1)
                ERR_EXIT("bind");
        }
        void Listen()
        {
            if(::listen(socket_fd_,SOMAXCONN)==-1)//SOMAXCONN defines the maximum number you're allowed to pass to listen()
                ERR_EXIT("listen");
        }

        int Accept()
        {
            int peerfd;
            if(peerfd=::accept(socket_fd_,NULL,NULL) == -1)
                ERR_EXIT("accept");
            return  peerfd;
        }

        void shutdownWrite()//关闭连接的写这一半，其他详见UNP1:P136
        {
            if(::shutdown(socket_fd_,SHUT_WR)==-1)
                ERR_EXIT("shutdow");
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
 * =================NODE3  close 与shutdown区别==========================
 * 1）close把描述符的引用计数减1,仅在该计数变0时才关闭套接字。shutdown可以不管引用计数就激发TCP的正常连接终止序列
 * 2）close终止读写两个方面的数据传送。TCP是全双工，shutdown可以关闭读半边（SHUT_RD）/写半边（SHUT_WR）/两边都关闭（SHUT_RDWR）
 * ===============思考====================
 */
