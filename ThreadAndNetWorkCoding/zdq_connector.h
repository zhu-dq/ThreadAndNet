/*
 * Connector类
 * 作用：在非阻塞网络编程中，发起连接的基本方式是调用connect,当socket变得可写时表明连接建立完毕.
 *      当然这其中要处理各种各样的错误，因此我们要封装成Connector 类，同时提供restat功能
 * 几个设计点：
 *          1. socket是一次性的，一旦出错，就无法恢复，只能关闭重来。但是Connector是可以重复使用的，因此每次尝试连接
 *             都要使用新的socket文件描述符和新的Channel对象。要留意Channel对象的生命周期管理，并防止socket文件描述符泄漏。
 *          2. 当connect发生错误时，重试的间隔应该逐渐延长
 *          3. 处理自连接：断开连接再重试
 * 作者：zhu-dq
 */
#ifndef ZDQ_CONNECTOR_H
#define ZDQ_CONNECTOR_H

#include <functional>
#include <memory>
#include "zdq_noncopyable.h"
#include "zdq_inet_address.h"
#include "zdq_socket.h"

namespace ZDQ{

    class EventLoop;
    class Channel;

    class Connector : ZDQ::NonCopyable,
                      public std::enable_shared_from_this<Connector> {
    public:
        typedef  std::function<void (int sockfd)> NewConnectionCallback;
        Connector(EventLoop * loop,const InetAddress & serverAddr);
        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback & cb){newConnectionCallback_ = cb;}

        void start();
        void stop();
        void restart();

    private:
        enum States { kDisconnected, kConnecting, kConnected };
        static const int kMaxRetryDelayMs = 30*1000;
        static const int kInitRetryDelayMs = 500;
        void setState(States s) { state_ = s; }
        void startInLoop();
        void stopInLoop();
        void connect();
        void connecting(int sockfd);
        void handleWrite();
        void handleError();
        int removeAndResetChannel();
        void resetChannel();
        void retry(int sockfd);
        EventLoop* loop_;
        InetAddress serverAddr_;
        bool connect_;
        States state_;
        std::shared_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        int retryDelayMs_;
    };
}

#endif
