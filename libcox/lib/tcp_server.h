#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "inner_pre.h"
#include "tcp_callbacks.h"
#include "listener.h"
#include <stack>

class EventLoop;

// We can use this class to create a TCP server.
// The typical usage is :
//      1. Create a TCPServer object
//      2. Set the message callback and connection callback
//      3. Call TCPServer::Init()
//      4. Call TCPServer::Start()
//      5. Process TCP client connections and messages in callbacks
//      6. At last call Server::Stop() to stop the whole server
//
// The examples code is as bellow:
// <code>
//     std::string addr = "0.0.0.0:9099";
//     int thread_num = 4;
//     evpp::EventLoop loop;
//     evpp::TCPServer server(&loop, addr, "TCPEchoServer", thread_num);
//     server.SetMessageCallback([](const evpp::TCPConnPtr& conn,
//                                  evpp::Buffer* msg) {
//         // Do something with the received message
//         conn->Send(msg); // At here, we just send the received message back.
//     });
//     server.SetConnectionCallback([](const evpp::TCPConnPtr& conn) {
//         if (conn->IsConnected()) {
//             LOG_INFO << "A new connection from " << conn->remote_addr();
//         } else {
//             LOG_INFO << "Lost the connection from " << conn->remote_addr();
//         }
//     });
//     server.Init();
//     server.Start();
//     loop.Run();
// </code>
//
class TCPServer 
{
    public:
        TCPServer(EventLoop* loop, const unsigned short port, const char *ip, bool reuse, int num);
        ~TCPServer();
        //void Init();
        void Start();
        EventLoop* GetLoop() const
        {
            return loop_;
        }
        struct Argument
        {
            void* arg;
            CoRoutine* co;
        };
    public:
        void SetConnectionCallback(const ConnectionCallback& cb)
        {
            connectionCallback_ = cb;
        }
        void SetMessageCallback(MessageCallback cb)
        {
            messageCallback_ = cb;
        }
    private:
        static void* HandleIO(void *);
        //void HandleIO(void *);
        int CreateTcpSocket(const unsigned short port, const char *ip, bool reuse);
    private:
        EventLoop* loop_;  // the listening loop
        Listener listener_;
        const int num_;
        //std::stack<CoRoutine *> pool;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
};

#endif
