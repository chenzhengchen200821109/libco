#include "inner_pre.h"
#include "tcp_conn.h"
#include "event_loop.h"
#include "connector.h"
#include "sockets.h"
#include "buffer.h"
#include "utility.h"


TCPConn::TCPConn(EventLoop* loop,
                Connector* connector)
    : loop_(loop)
    , owner_(connector)
    , fd_(owner_->GetFd())
{
    //DLOG_TRACE << "TCPConn::[" << name_ << "] channel=" << chan_.get() << " fd=" << sockfd << " addr=" << AddrToString();
}

TCPConn:: ~TCPConn() 
{
    //DLOG_TRACE << "name=" << name()
}

void TCPConn::Close() 
{
    //DLOG_TRACE << "fd=" << fd_ << " status=" << StatusToString() << " addr=" << AddrToString();
}

void TCPConn::Send(const void* data, size_t len)
{
    Send(Slice(static_cast<const char *>(data), len));
}

void TCPConn::Send(const std::string& d) 
{
    SendStringInLoop(d);
}

void TCPConn::Send(const Slice& message)
{
    SendInLoop(message);
}

void TCPConn::Send(Buffer* buf)
{
    SendInLoop(buf->peek(), buf->readableBytes());
}

void TCPConn::SendInLoop(const Slice& message)
{
    SendInLoop(message.data(), message.size());
}

void TCPConn::SendStringInLoop(const std::string& message)
{
    SendInLoop(message.data(), message.size());
}

void TCPConn::SendInLoop(const void* data, size_t len)
{
    // data transfer between client and internal buffer
    // should be all right
    outputBuffer_.append(static_cast<const char *>(data), len);
 
}

void TCPConn::HandleRead(int sockfd)
{
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
    else {
        close(sockfd);
        owner_->SetFd(-1);
    }

}

void TCPConn::HandleClose(int sockfd)
{
    closeCallback_(shared_from_this());
    //DLOG_TRACE << "addr=" << AddrToString() << " fd=" << fd_ << " status_=" << StatusToString();
    close(sockfd);
    owner_->SetFd(-1); 
}

//void DelayClose();
void TCPConn::HandleError(int sockfd)
{
    //DLOG_TRACE << "fd=" << fd_ << " status=" << StatusToString();
    HandleClose(sockfd); 
}

void TCPConn::ConnectEstablished(int sockfd)
{
    //
    connectionCallback_(shared_from_this());
    HandleWrite(sockfd);    
    if (owner_->GetFd() != -1)
    {
        HandleRead(sockfd);
    }
}
