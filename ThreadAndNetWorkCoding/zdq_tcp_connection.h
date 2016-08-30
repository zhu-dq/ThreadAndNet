/*
 * TcpConnection类
 * 作用：处理一次TCP连接,对应一个connfd
 * 作者：zhu-dq
 */
#ifndef ZDQ_TCP_CONNECTION_H
#define ZDQ_TCP_CONNECTION_H

#include "zdq_socket.h"
#include "zdq_timestamp.h"
#include <memory>
#include <string>
#include "zdq_buffer.h"
//#include "zdq_event_loop.h"

using namespace std;

namespace ZDQ{
    class EventLoop;
    class Channel;
    class TcpConnection:public ZDQ::NonCopyable,
                        public std::enable_shared_from_this<TcpConnection>
    {
    public:
        enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        typedef std::function<void (const TcpConnectionPtr&)> ConnCallback;
        //typedef std::function<void (const TcpConnectionPtr&, string, Timestamp)> MessageCallback;
        typedef std::function<void (const TcpConnectionPtr&, Buffer * buf, Timestamp)> MessageCallback;
        typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
        typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
        TcpConnection(EventLoop* loop,
                      const string& name,
                      int sockfd,
                      const InetAddress& localAddr,
                      const InetAddress& peerAddr);

        ~TcpConnection(){};


        void setConnectionCallback(const ConnCallback& cb) { connCallback_ = cb; }

        void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

        void setCloseCallback(const CloseCallback& cb) {closeCallback_ = cb;}

        void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

        void setState(StateE s) { state_ = s; }

        bool connected() const { return state_ == kConnected; }

        const string& name() const { return name_; }
        EventLoop * getLoop() const { return loop_; }

        const InetAddress& localAddress() const { return localAddr_; }

        const InetAddress& peerAddress() const { return peerAddr_; }

        void connectEstablished();//[开始]  第一次建立时调用

        void connectDestroyed();//[结束]    TcpConnection被删除的时候调用

        void send(const std::string & message); //A

        void shutdown();  //B

        void forceClose();

    private:
        StateE  state_;
        //handleRead 检查read 返回值，根据返回值调用 messageCallback_ handleClose handleError等
        void handleRead(Timestamp);
        void handleWrite();
        void handleClose();
        void handleError();
        void sendInLoop(const std::string& message);//A
        void shutdownInLoop();//B
        void forceCloseInLoop();
        EventLoop * loop_;
        std::string name_;
        std::unique_ptr<ZDQ::Socket> socket_;
        std::unique_ptr<ZDQ::Channel> channel_;
        InetAddress localAddr_;
        InetAddress peerAddr_;
        ConnCallback connCallback_;
        MessageCallback messageCallback_;
        CloseCallback closeCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        Buffer inputBuf_;
        Buffer outputBuf_;
    };
}

#endif
