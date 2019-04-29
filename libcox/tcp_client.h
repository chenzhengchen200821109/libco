#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__ 

#include "inner_pre.h"
#include "event_loop.h"
#include "tcp_callbacks.h"
#include "tcp_conn.h"
#include "connector.h"
//#include "evpp/any.h"
#include <map>
#include <atomic>
#include <mutex>

// We can use this class to create a TCP client.
// The typical usage is :
//      1. Create a TCPClient object
//      2. Set the message callback and connection callback
//      3. Call TCPClient::Connect() to try to establish a connection with remote server
//      4. Use TCPClient::Send(...) to send messages to remote server
//      5. Handle the connection and messages in callbacks
//      6. Call TCPClient::Disonnect() to disconnect from remote server
//
class TCPClient {
    public:
        // @brief The constructor of the class
        // @param[in] loop - The EventLoop runs this object
        // @param[in] remote_addr - The remote server address with format "host:port"
        //  If the host is not IP, it will automatically do the DNS resolving asynchronously
        // @param[in] name -
        TCPClient(EventLoop* loop, const std::string& remote_addr/*host:port*/, const std::string& name)
            : loop_(loop)
            , remote_addr_(raddr)
            , name_(name)
            , conn_fn_(&DefaultConnectionCallback)
            , msg_fn_(&DefaultMessageCallback) 
        {
            //DLOG_TRACE << "remote addr=" << raddr;
        }

        ~TCPClient() 
        {
            DLOG_TRACE;
            assert(!connector_.get());
            auto_reconnect_.store(false);
            TCPConnPtr c = conn();
            if (c) {
                // Most of the cases, the conn_ is at disconnected status at this time.
                // But some times, the user application layer will call TCPClient::Close()
                // and delete TCPClient object immediately, that will make conn_ to be at disconnecting status.
                assert(c->IsDisconnected() || c->IsDisconnecting());
                if (c->IsDisconnecting()) {
                    // the reference count includes :
                    //  - this 
                    //  - c 
                    //  - A disconnecting callback which hold a shared_ptr of TCPConn 
                    assert(c.use_count() >= 3); 
                    c->SetCloseCallback(CloseCallback());
                }
            }
            conn_.reset(); 
        }

        // @brief We can bind a local address. This is an optional operation.
        //  If necessary, it should be called before doing Connect().
        // @param[IN] local_addr -
        void Bind(const std::string& local_addr/*host:port*/)
        {
            local_addr_ = local_addr
        }

        // @brief Try to establish a connection with remote server asynchronously
        //  If the connection callback is set properly it will be invoked when
        //  the connection is established successfully or timeout or cannot
        //  establish a connection.
        void Connect() 
        {
            //LOG_INFO << "remote_addr=" << remote_addr();
            auto f = [this]() {
                assert(loop_->IsInLoopThread());
                connector_.reset(new Connector(loop_, this));
                connector_->SetNewConnectionCallback(std::bind(&TCPClient::OnConnection, this, std::placeholders::_1, std::placeholders::_2));
                connector_->Start();
            };
            loop_->RunInLoop(f);
        }

        // @brief Disconnect from the remote server. When the connection is
        //  broken down, the connection callback will be invoked.
        void Disconnect()
        {
            //DLOG_TRACE;
            loop_->RunInLoop(std::bind(&TCPClient::DisconnectInLoop, this));
        }
    public:
        // Set a connection event relative callback when the TCPClient
        // establishes a connection or an exist connection breaks down or failed to establish a connection.
        // When these three events happened, the value of the parameter in the callback is:
        //      1. Successfully establish a connection : TCPConn::IsConnected() == true
        //      2. An exist connection broken down : TCPConn::IsDisconnecting() == true
        //      3. Failed to establish a connection : TCPConn::IsDisconnected() == true and TCPConn::fd() == -1
        void SetConnectionCallback(const ConnectionCallback& cb)
        {
            conn_fn_ = cb;
            auto  c = conn();
            if (c) {
                c->SetConnectionCallback(cb);
            }
        }

    // Set the message callback to handle the messages from remote server
    void SetMessageCallback(const MessageCallback& cb) {
        msg_fn_ = cb;
    }

public:
    bool auto_reconnect() const 
    {
        return auto_reconnect_;
    }
    void set_auto_reconnect(bool v) 
    {
        auto_reconnect_.store(v);
    }
    //Duration reconnect_interval() const 
    //{
    //    return reconnect_interval_;
    //}
    //void set_reconnect_interval(Duration timeout) {
    //    reconnect_interval_ = timeout;
    //}
    //Duration connecting_timeout() const {
    //    return connecting_timeout_;
    //}
    //void set_connecting_timeout(Duration timeout) {
    //    connecting_timeout_ = timeout;
    //}
    //void set_context(const Any& c) {
    //    context_ = c;
    //}
    //const Any& context() const {
    //    return context_;
    //}
    TCPConnPtr conn() const
    {
        if (loop_->IsInLoopThread()) {
            return conn_;
        } else {
            // If it is not in the loop thread, we should add a lock here 
            std::lock_guard<std::mutex> guard(mutex_);
            TCPConnPtr c = conn_;
            return c;
        }
    }

    // Return the remote address with the format of 'host:port'
    const std::string& remote_addr() const 
    {
        return remote_addr_;
    }
    const std::string& local_addr() const 
    {
        return local_addr_;
    }
    const std::string& name() const 
    {
        return name_;
    }
    EventLoop* loop() const 
    {
        return loop_;
    }
private:
    void DisconnectInLoop()
    {
        LOG_WARN << "TCPClient::DisconnectInLoop this=" << this << " remote_addr=" << remote_addr_;
        assert(loop_->IsInLoopThread());
        auto_reconnect_.store(false);
        if (conn_) {
            //DLOG_TRACE << "Close the TCPConn " << conn_.get() << " status=" << conn_->StatusToString();
            assert(!conn_->IsDisconnected() && !conn_->IsDisconnecting());
            conn_->Close();
        } else {
            // When connector_ is connecting to the remote server ...
            assert(connector_ && !connector_->IsConnected());
        }
        if (connector_->IsConnected() || connector_->IsDisconnected()) {
            //DLOG_TRACE << "Nothing to do with connector_, Connector::status=" << connector_->status();
        } else {
            // When connector_ is trying to connect to the remote server we should cancel it to release the resources.
            connector_->Cancel();
        }   
        connector_.reset(); 
    }
    void OnConnection(evpp_socket_t sockfd, const std::string& laddr)
    {
        if (sockfd < 0) {
            //DLOG_TRACE << "Failed to connect to " << remote_addr_ << ". errno=" << errno << " " << strerror(errno);
            // We need to notify this failure event to the user layer
            // Note: When we could not connect to a server,
            //       the user layer will receive this notification constantly
            //       because the connector_ will retry to do reconnection all the time.
            conn_fn_(TCPConnPtr(new TCPConn(loop_, "", sockfd, laddr, remote_addr_, 0)));
            return;
        }
        //DLOG_TRACE << "Successfully connected to " << remote_addr_;
        assert(loop_->IsInLoopThread());
        TCPConnPtr c = TCPConnPtr(new TCPConn(loop_, name_, sockfd, laddr, remote_addr_, id++));
        c->set_type(TCPConn::kOutgoing);
        c->SetMessageCallback(msg_fn_);
        c->SetConnectionCallback(conn_fn_);
        c->SetCloseCallback(std::bind(&TCPClient::OnRemoveConnection, this, std::placeholders::_1));
        {
            std::lock_guard<std::mutex> guard(mutex_);
            conn_ = c;
        }
        c->OnAttachedToLoop();
    }
    void OnRemoveConnection(const TCPConnPtr& conn)
    {
        assert(c.get() == conn_.get());
        assert(loop_->IsInLoopThread());
        conn_.reset();
        if (auto_reconnect_.load()) {
            Reconnect();
        }
    }
    void Reconnect()
    {
        //DLOG_TRACE << "Try to reconnect to " << remote_addr_ << " in " << reconnect_interval_.Seconds() << "s again";
        Connect();
    }
private: 
    EventLoop* loop_;
    std::string local_addr_; // If the local address is not empty, we will bind to this local address before doing connect()
    std::string remote_addr_; // host:port
    std::string name_;
    std::atomic<bool> auto_reconnect_ = { true }; // The flag whether it reconnects automatically, Default : true
    //Duration reconnect_interval_ = Duration(3.0); // Default : 3 seconds 
    //Any context_;
    mutable std::mutex mutex_; // The guard of conn_
    TCPConnPtr conn_;
    std::shared_ptr<Connector> connector_;
    //Duration connecting_timeout_ = Duration(3.0); // Default : 3 seconds
    ConnectionCallback conn_fn_;
    MessageCallback msg_fn_;
};

#endif
