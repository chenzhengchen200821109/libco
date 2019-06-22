#include <atomic>
#include "inner_pre.h"
#include "tcp_client.h"
#include "connector.h"
#include "inetaddress.h"
#include <errno.h>
#include <stdio.h>
#include <iostream>

TCPClient::TCPClient(EventLoop* loop, const InetAddress& serverAddr, int NumCo = 100, const std::string& name = "tcpclient")
    : loop_(loop)
    , NumCo_(NumCo)
    , name_(name)
{
    DLOG_TRACE;

    for (int i = 0; i < NumCo_; i++) 
    {
        ConnectorPtr con(new Connector(loop, serverAddr));
        TCPConnPtr conn(new TCPConn(loop, con.get()));
        con->setNewConnectionCallback(std::bind(&TCPClient::newConnection, this, std::placeholders::_1));
        connectors_.push_back(con);
        connections_.push(conn);
    }
}

TCPClient::~TCPClient() 
{
    DLOG_TRACE;
}

void TCPClient::Start()
{
    //Connect();
    DLOG_TRACE;
    for (size_t i = 0; i < connectors_.size(); i++) 
    {
        connectors_[i]->Start();
    }
}

void TCPClient::newConnection(int sockfd)
{
    DLOG_TRACE;

    TCPConnPtr conn = connections_.front();
    connections_.pop();
    connections_.push(conn);
    conn->SetConnectionCallback(connectionCallback_);
    conn->SetMessageCallback(messageCallback_);
    conn->SetWriteCompleteCallback(writeCompleteCallback_);
    conn->SetCloseCallback(closeCallback_);
    conn->ConnectEstablished(sockfd);
}
