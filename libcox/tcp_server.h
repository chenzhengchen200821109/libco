#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "inner_pre.h"
#include "event_loop.h"
#include "event_loop_thread_pool.h"
#include "tcp_callbacks.h"

#include "thread_dispatch_policy.h"
#include "server_status.h"

#include <map>

class Listener;

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
class TCPServer : public ThreadDispatchPolicy, public ServerStatus {
    public:
        typedef std::function<void()> DoneCallback;

        // @brief The constructor of a TCPServer.
        // @param loop -
        // @param listen_addr - The listening address with "ip:port" format
        // @param name - The name of this object
        // @param thread_num - The working thread count
        TCPServer(EventLoop* loop,
                  const std::string& listen_addr/*ip:port*/,
                  const std::string& name,
                  uint32_t thread_num)
            : loop_(loop)
            , listen_addr_(listen_addr)
            , name_(name)
            , conn_fn_(&DefaultConnectionCallback)
            , msg_fn_(&DefaultMessageCallback)
            , next_conn_id_(0)
        {
            //DLOG_TRACE << "name=" << name << " listening addr " << laddr << " thread_num=" << thread_num;
            tpool_.reset(new EventLoopThreadPool(loop_, thread_num));    
        }

        ~TCPServer()
        {
            //DLOG_TRACE;
            assert(connections_.empty());
            assert(!listener_);
            if (tpool_) {
                assert(tpool_->IsStopped());
                tpool_.reset();
            }
        }

        // @brief Do the initialization works here.
        //  It will create a nonblocking TCP socket, and bind with the give address
        //  then listen on it. If there is anything wrong it will return false.
        // @return bool - True if anything goes well
        bool Init()
        {
            //DLOG_TRACE;
            assert(status_ == kNull);
            listener_.reset(new Listener(loop_, listen_addr_));
            listener_->Listen(); // listen loop
            status_.store(kInitialized);
            return true;
        }

        // @brief Start the TCP server and we can accept new connections now.
        // @return bool - True if anything goes well
        bool Start()
        {
            //DLOG_TRACE;
            assert(status_ == kInitialized);
            status_.store(kStarting);
            assert(listener_.get()); // listener is still alive
            bool rc = tpool_->Start(true);

            if (rc) {
                assert(tpool_->IsRunning());
                listener_->SetNewConnectionCallback(
                        std::bind(&TCPServer::HandleNewConn,
                        this,
                        std::placeholders::_1,
                        std::placeholders::_2,
                        std::placeholders::_3));
                // We must set status_ to kRunning firstly and then we can accept new
                // connections. If we use the following code :
                //     listener_->Accept();
                //     status_.store(kRunning);
                // there is a chance : we have accepted a connection but status_ is not 
                // kRunning that will cause the assert(status_ == kRuning) failed in  
                // TCPServer::HandleNewConn. 
                status_.store(kRunning);
                listener_->Accept();

            }
            return rc;
        }

        // @brief Stop the TCP server
        // @param cb - the callback cb will be invoked when
        //  the TCP server is totally stopped
        void Stop(DoneCallback cb = DoneCallback())
        {
            //DLOG_TRACE << "Entering ...";
            assert(status_ == kRunning);
            status_.store(kStopping);
            substatus_.store(kStoppingListener);
            loop_->RunInLoop(std::bind(&TCPServer::StopInLoop, this, on_stopped_cb));
        }

        // @brief Reinitialize some data fields after a fork
        //void AfterFork();

    public:
        // Set a connection event relative callback when the TCPServer
        // receives a new connection or an exist connection breaks down.
        // When these two events happened, the value of the parameter in the callback is:
        //      1. Received a new connection : TCPConn::IsConnected() == true
        //      2. An exist connection broken down : TCPConn::IsDisconnecting() == true
        void SetConnectionCallback(const ConnectionCallback& cb) 
        {
            conn_fn_ = cb;
        }

        // Set the message callback to handle the messages from remote client
        void SetMessageCallback(MessageCallback cb) 
        {
            msg_fn_ = cb;
        }

    public:
        const std::string& listen_addr() const 
        {
            return listen_addr_;
        }
    private:
        void StopThreadPool();
        {
            //DLOG_TRACE << "pool=" << tpool_.get();
            assert(loop_->IsInLoopThread());
            assert(IsStopping());
            substatus_.store(kStoppingThreadPool);
            tpool_->Stop(true);
            assert(tpool_->IsStopped());
            // Make sure all the working threads totally stopped.
            tpool_->Join();
            tpool_.reset();
            substatus_.store(kSubStatusNull);
        }
        void StopInLoop(DoneCallback on_stopped_cb)
        {
            //DLOG_TRACE << "Entering ...";
            assert(loop_->IsInLoopThread());
            listener_->Stop();
            listener_.reset();
            if (connections_.empty()) {
                // Stop all the working threads now.
                //DLOG_TRACE << "no connections";
                StopThreadPool();
                if (on_stopped_cb) {
                    on_stopped_cb();
                    on_stopped_cb = DoneCallback();
                }
                status_.store(kStopped);
            } else {
                //DLOG_TRACE << "close connections";
                for (auto& c : connections_) {
                    if (c.second->IsConnected()) {
                        //DLOG_TRACE << "close connection id=" << c.second->id() << " fd=" << c.second->fd();
                        c.second->Close();
                    } else {
                        //DLOG_TRACE << "Do not need to call Close for this TCPConn it may be doing disconnecting. TCPConn=" << c.second.get() << " fd=" << c.second->fd() << " status=" << StatusToString();
                    }
                }
                stopped_cb_ = on_stopped_cb;
                // The working threads will be stopped after all the connections closed.
            }
            //DLOG_TRACE << "exited, status=" << StatusToString();
        }

        void RemoveConnection(const TCPConnPtr& conn)
        {
            //DLOG_TRACE << "conn=" << conn.get() << " fd="<< conn->fd() << " connections_.size()=" << connections_.size();
            auto f = [this, conn]() {
                // Remove the connection in the listening EventLoop 
                //DLOG_TRACE << "conn=" << conn.get() << " fd="<< conn->fd() << " connections_.size()=" << connections_.size();
                assert(this->loop_->IsInLoopThread());
                this->connections_.erase(conn->id());
                if (IsStopping() && this->connections_.empty()) {
                    // At last, we stop all the working threads 
                    //DLOG_TRACE << "stop thread pool";
                    assert(substatus_.load() == kStoppingListener);
                    StopThreadPool();
                    if (stopped_cb_) {
                        stopped_cb_();
                        stopped_cb_ = DoneCallback();
                    }
                    status_.store(kStopped);
                }
            };
            loop_->RunInLoop(f);
        }

        void HandleNewConn(evpp_socket_t sockfd, const std::string& remote_addr/*ip:port*/, const struct sockaddr_in* raddr)
        {
            //DLOG_TRACE << "fd=" << sockfd;
            assert(loop_->IsInLoopThread());
            if (IsStopping()) {
                //LOG_WARN << "this=" << this << " The server is at stopping status. Discard this socket fd=" << sockfd << " remote_addr=" << remote_addr;
                close(sockfd);
                return;
            }
            assert(IsRunning());
            EventLoop* into_loop = GetNextLoop(raddr);
        #ifdef H_DEBUG_MODE 
            std::string n = name_ + "-" + remote_addr + "#" + std::to_string(next_conn_id_);
        #else 
            std::string n = remote_addr;
        #endif 
            ++next_conn_id_;
            TCPConnPtr conn(new TCPConn(io_loop, n, sockfd, listen_addr_, remote_addr, next_conn_id_));
            assert(conn->type() == TCPConn::kIncoming);
            conn->SetMessageCallback(msg_fn_);
            conn->SetConnectionCallback(conn_fn_);
            conn->SetCloseCallback(std::bind(&TCPServer::RemoveConnection, this, std::placeholders::_1));
            io_loop->RunInLoop(std::bind(&TCPConn::OnAttachedToLoop, conn));
            connections_[conn->id()] = conn;
        }

        EventLoop* GetNextLoop(const struct sockaddr_in* raddr)
        {
            if (IsRoundRobin()) {
                return tpool_->GetNextLoop();
            } else {
                return tpool_->GetNextLoopWithHash(raddr->sin_addr.s_addr);
            }
        }
    private:
        EventLoop* loop_;  // the listening loop
        const std::string listen_addr_; // ip:port
        const std::string name_;
        std::unique_ptr<Listener> listener_;
        std::shared_ptr<EventLoopThreadPool> tpool_;
        ConnectionCallback conn_fn_;
        MessageCallback msg_fn_;

        DoneCallback stopped_cb_;

        // always in the listening loop thread
        uint64_t next_conn_id_ = 0;
        typedef std::map<uint64_t/*the id of the connection*/, TCPConnPtr> ConnectionMap;
        ConnectionMap connections_;
};

#endif
