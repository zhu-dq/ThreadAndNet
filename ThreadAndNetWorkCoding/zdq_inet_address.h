/*
 * 封装 IPV4 套接字  struct sockaddr_in 
 * 知识点：见最下NODE
 * 作者：zhu-dq
 */

#ifndef ZDQ_INET_ADDRESS_H
#define ZDQ_INET_ADDRESS_H

#include <sys/socket.h>
#include <netinet/in.h>//struct sockaddr_in
#include <cstring>     //memset
#include <string>
#include <arpa/inet.h> //inet_ntoa

/*
 *  perror 这个函数在系统调用失败的时候调用，它在标准错误上显示关于错误的消息，然后退出系统
 */

#define ERR_EXIT(m) \
    do { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

typedef struct sockaddr SA;//struct sockaddr是IPV4通用的套接字地址

namespace ZDQ{
    class InetAddress{

    public:

        explicit InetAddress(int port)
        {
            /*
             * sock_addr_结构详见UNP1:P56
             */
            ::memset(&sock_addr_,0, sizeof(sock_addr_));
            sock_addr_.sin_family = AF_INET;//AF_INET:IPV4协议 其他family常量详见UNP1:P78
            sock_addr_.sin_addr.s_addr = INADDR_ANY;//IPV4地址，INADDR_ANY见下面NODE3
            /*
            if(inet_pton(AF_INET,"127.0.0.1",&sock_addr_.sin_addr)<=0)
                ERR_EXIT("inet_pton error");*/
            //sock_addr_.sin_port = port;//端口号
            sock_addr_.sin_port = htons(port);//端口号
        }
        InetAddress(std::string ip , int port)
        {
            ::memset(&sock_addr_,0, sizeof(sock_addr_));
            sock_addr_.sin_family = AF_INET;//AF_INET:IPV4协议
            if(inet_pton(AF_INET,ip.c_str(),&sock_addr_.sin_addr)<=0)//ip
                ERR_EXIT("inet_pton error");
            sock_addr_.sin_port = htons(port);//port

        }
        InetAddress(const struct sockaddr_in & addr):sock_addr_(addr){}

        void setSockAddrInet(const struct sockaddr_in & addr)
        {
            sock_addr_ = addr;
        }

        const struct sockaddr_in * getSockAddrInet()const
        {
            return & sock_addr_;
        }

        uint32_t getIpAddress()const//原始
        {
            return sock_addr_.sin_addr.s_addr;
        }

        uint16_t getPort()const//原始
        {
            return sock_addr_.sin_port;
        }

        std::string getIpAsString()const
        {
            return std::string(inet_ntoa(sock_addr_.sin_addr));//inet_ntoa:转换成点分十进制
        }

        uint16_t getPort_h()const
        {
            return ntohs(sock_addr_.sin_port);//ntohs:网络字节序转主机字节序
        }

        static InetAddress getLocalInetAddress(int); //封装getsockname,获取与某个套接字关联的本地协议地址
        static InetAddress getPeerInetAddress(int);//封装getpeername,获取与某个套接字关联的外地地协议地址
    private:
        struct sockaddr_in sock_addr_;//结构详见UNP1:P57
    };

    inline InetAddress InetAddress::getLocalInetAddress(int sockfd)
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        if(::getsockname(sockfd,(SA*)&addr,&len)==-1)
            ERR_EXIT("getsockname");
        return InetAddress(addr);
    }

    inline InetAddress InetAddress::getPeerInetAddress(int sockfd)
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        if(::getpeername(sockfd,(SA*)&addr,&len)==-1)
            ERR_EXIT("getpeername");
        return  InetAddress(addr);
    }
}


#endif


/*
 *  ========NODE1：struct sockaddr 与 struct sockaddr_in区别与联系================
 *  struct sockaddr是通用的套接字地址，
 *  而struct sockaddr_in则是internet环境下套接字的地址形式，
 *  二者长度一样，都是16个字节。二者是并列结构，
 *  指向sockaddr_in结构的指针也可以指向sockaddr。
 *  一般情况下，需要把sockaddr_in结构强制转换成sockaddr结构再传入系统调用函数中。
 *  其他详细UNP1:P61
 *  =========NODE2:点分十进制与网络字节序二进制值转换=======================
 *  IPV4/IPV6: 点分二进制 ---inet_pton---->网络字节序二进制
 *  IPV4/IPV6: 网络字节序二进制 ---inet_ntop---->点分二进制
 *  IPV4: x.x.x.x ---inet_aton/inet_addr---> 10010110......
 *  IPV4: 10010110......--->inet_ntoa---> x.x.x.x
 *  其他详见UNP1：P69
 *  =========NODE2:主机字节序与网络字节序转换=======================
 *  大端：假设16bit:[8位][8位]  --->   [高序字节][低序字节]  PS:内存地址-->
 *  小端：假设16bit:[8位][8位]  --->   [低序字节][高序字节]  PS:内存地址-->
 *  问题：上述两种内存存储方法都有系统在用
 *  主机字节序：我们把某个给定系统所用的字节序称为主机字节序
 *  网络字节序：为了统一，网络协议必须指定一个网络字节序
 *  主机字节序  ----htons(uint16_t)/htonl(uint32_t)--->  网络字节序
 *  网络字节序  ----ntons(uint16_t)/ntonl(uint32_t)--->  主机字节序
 *  其他详见UNP1:P65
 *  =========NODE3:INADDR_ANY=======================
 *  对于IPV4而言，通配地址由常量INADDR_ANY指定，其值一般为0。告诉内核去选择IP地址,常用于服务端
 *  其他详见UNP1:P83
 */
