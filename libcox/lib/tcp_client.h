#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__ 

#include "inner_pre.h"
#include "event_loop.h"
#include "connector.h"
#include "tcp_conn.h"
#include "tcp_callbacks.h"
#include <vector>
#include <string>
#include <queue>
#include "inetaddress.h"

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

// We can use this class to create a TCP client.
// The typical usage is :
//      1. Create a TCPClient object
//      2. Set the message callback and connection callback
//      3. Call TCPClient::Connect() to try to establish a connection with remote server
//      4. Use TCPClient::Send(...) to send messages to remote server
//      5. Handle the connection and messages in callbacks
//      6. Call TCPClient::Disonnect() to disconnect from remote server
//
class TCPClient 
{
    public:
        TCPClient(EventLoop* loop, const InetAddress& serverAddr, int NumCo, const std::string& name);
        ~TCPClient(); 
        void Start();
        void setConnectionCallback(const ConnectionCallback& cb)
        {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb)
        {
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb)
        {
            writeCompleteCallback_ = cb;
        }
        void setCloseCallback(const CloseCallback& cb)
        {
            closeCallback_ = cb;
        }
        std::string Name() const
        {
            return name_;
        }
    private: 
        //void Connect();
        void newConnection(int sockfd);
    private:
        EventLoop* loop_;
        int NumCo_;
        const std::string name_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;
        CloseCallback closeCallback_;

        std::vector<ConnectorPtr> connectors_;
        std::queue<TCPConnPtr> connections_;
};

#endif
