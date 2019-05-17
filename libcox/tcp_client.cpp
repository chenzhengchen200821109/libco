#include <atomic>
#include "inner_pre.h"
#include "tcp_client.h"
#include "connector.h"
#include <errno.h>
#include <stdio.h>
#include <iostream>

TCPClient::TCPClient(EventLoop* loop, const char *ip, const unsigned short port, int NumCo, const std::string& name)
    : loop_(loop)
    //, connector_(Connector(loop, ip, port))
    , NumCo_(NumCo)
    , name_(name)
{
    //DLOG_TRACE << "remote addr=" << raddr;
    std::cout << "TCPClient::TCPClient()" << std::endl;
    for (int i = 0; i < NumCo_; i++) {
        //Connector con(loop, ip, port);
        ConnectorPtr con(new Connector(loop, ip, port));
        TCPConnPtr conn(new TCPConn(loop, con.get()));
        con->setNewConnectionCallback(std::bind(&TCPClient::newConnection, this, std::placeholders::_1));
        //connectors_.push_back(std::move(con));
        connectors_.push_back(con);
        connections_.push(conn);
    }
    //connector_.setNewConnectionCallback(std::bind(&TCPClient::newConnection, this, std::placeholders::_1));
}

TCPClient::~TCPClient() 
{
    //DLOG_TRACE;
    std::cout << "TCPClient::~TCPClient()" << std::endl;
}

void TCPClient::Start()
{
    //Connect();
    for (size_t i = 0; i < connectors_.size(); i++) {
        connectors_[i]->Start();
    }
}

//void TCPClient::Connect() 
//{
    //LOG_INFO << "remote_addr=" << remote_addr();
//    std::cout << "TCPClient::Connect()" << std::endl;
    //connector_->Connect();
    //connector_->Start();
//}

void TCPClient::newConnection(int sockfd)
{
    TCPConnPtr conn = connections_.front();
    connections_.pop();
    connections_.push(conn);
    conn->SetConnectionCallback(connectionCallback_);
    conn->SetMessageCallback(messageCallback_);
    conn->SetWriteCompleteCallback(writeCompleteCallback_);
    //conn->setCloseCallback();
    conn->ConnectEstablished(sockfd);
}
