#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__

#include <memory>
#include "inner_pre.h"
#include "buffer.h"
#include "tcp_callbacks.h"
#include "slice.h"

class EventLoop;
class Connector;

class TCPConn : public std::enable_shared_from_this<TCPConn> 
{
    public:
        TCPConn(EventLoop* loop, Connector* connector);
        ~TCPConn(); 

        void Close();

        //void Send(const char* s) 
        //{
        //    Send(s, strlen(s));
        //}
        void Send(const void* data, size_t len);
	    void Send(const std::string& str); 
        //void Send(const Slice& message);
        //void Send(Buffer* buf);
    public:
        int GetFd() const 
        {
            return fd_;
        }
    public:
        void SetWriteCompleteCallback(const WriteCompleteCallback cb) 
        {
            writeCompleteCallback_ = cb;
        }
        void SetMessageCallback(MessageCallback cb) 
        {
            messageCallback_ = cb;
        }
        void SetConnectionCallback(ConnectionCallback cb) 
        {
            connectionCallback_ = cb;
        }
        void SetCloseCallback(CloseCallback cb) 
        {
            closeCallback_ = cb;
        }
        void ConnectEstablished(int sockfd);
        bool IsDisconnected()
        {
            return state_ == kDisconnected;
        }
        bool IsConnected()
        {
            return state_ == kConnected;
        }
    private:
        void HandleRead(int sockfd);
        void HandleWrite(int sockfd);
        void HandleClose(int sockfd);
        void HandleError(int sockfd);
        void SendInLoop(const Slice& message);
        void SendInLoop(const void* data, size_t len);
        void SendStringInLoop(const std::string& message);
    private:
        enum State { kDisconnected, kConnected};
        void SetState(State s)
        {
            state_ = s;
        }
    private:
        EventLoop* loop_;
        Connector* owner_; // use smart pointer
        int fd_;
        State state_;
        Buffer inputBuffer_;
        Buffer outputBuffer_;

        ConnectionCallback connectionCallback_; // This will be called to the user application layer
        MessageCallback messageCallback_; // This will be called to the user application layer
        WriteCompleteCallback writeCompleteCallback_; // This will be called to the user application layer
        CloseCallback closeCallback_; // This will be called to TCPClient or TCPServer
};

#endif
