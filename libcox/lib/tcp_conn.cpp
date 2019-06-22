#include "inner_pre.h"
#include "tcp_conn.h"
#include "event_loop.h"
#include "connector.h"
#include "sockets.h"
#include "buffer.h"
#include "utility.h"
#include "logging.h"
#include <memory>


TCPConn::TCPConn(EventLoop* loop, Connector* connector)
    : loop_(loop)
    , owner_(connector)
    , co_(owner_->GetCoRoutine())
{
    DLOG_TRACE;
}

TCPConn:: ~TCPConn() 
{
    DLOG_TRACE;
}

void TCPConn::Close() 
{
    //DLOG_TRACE << "fd=" << fd_ << " status=" << StatusToString() << " addr=" << AddrToString();
}

void TCPConn::Send(const void* data, size_t len)
{
    //Send(Slice(static_cast<const char *>(data), len));
    SendInLoop(data, len);
}

void TCPConn::Send(const std::string& str) 
{
    DLOG_TRACE;
    SendStringInLoop(str);
}

void TCPConn::Send(const Slice& message)
{
    DLOG_TRACE;
    SendInLoop(message);
}

void TCPConn::Send(Buffer* buf)
{
    DLOG_TRACE;
    SendInLoop(buf->peek(), buf->readableBytes());
}

void TCPConn::SendInLoop(const Slice& message)
{
    DLOG_TRACE;
    SendInLoop(message.data(), message.size());
}

void TCPConn::SendStringInLoop(const std::string& str)
{
    DLOG_TRACE;
    SendInLoop(str.data(), str.size());
}

void TCPConn::SendInLoop(const void* data, size_t len)
{
    DLOG_TRACE;
    outputBuffer_.append(static_cast<const char *>(data), len);
 
}

void TCPConn::HandleRead(int sockfd)
{
    DLOG_TRACE;

    int savedErrno = 0;
    ssize_t n = inputBuffer_.readFd(sockfd, &savedErrno);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_);
    }
    else if (n == 0)
    {
        HandleClose(sockfd);
    }
    else
    {
        errno = savedErrno;
        HandleError(sockfd);
    }
}

void TCPConn::HandleWrite(int sockfd)
{
    DLOG_TRACE;

    co_enable_hook_sys();
    ssize_t n = write(sockfd, outputBuffer_.peek(), outputBuffer_.readableBytes());
    if (n > 0)
    {
        outputBuffer_.retrieve(n);
        if (outputBuffer_.readableBytes() == 0)
        {
            if (writeCompleteCallback_)
                writeCompleteCallback_(shared_from_this());
        }
    }
    else 
    {
        HandleError(sockfd);
    }

}

void TCPConn::HandleClose(int sockfd)
{
    DLOG_TRACE;

    if (closeCallback_)
        closeCallback_(shared_from_this());
    close(sockfd);
    co_->fd = -1; 
    SetState(kDisconnected);
}

//void DelayClose();
void TCPConn::HandleError(int sockfd)
{
    DLOG_TRACE;
    HandleClose(sockfd); 
}

void TCPConn::ConnectEstablished(int sockfd)
{
    DLOG_TRACE;

    if (IsDisconnected()) 
    {
        connectionCallback_(shared_from_this());
        SetState(kConnected);
    }
    if (IsConnected()) 
    {
        // write data to remote peer
        HandleWrite(sockfd);    
        if (co_->fd != -1)
        {
            HandleRead(sockfd);
        }
    }
}
