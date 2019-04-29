#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <cstring>
#include <vector>
#include "inner_pre.h"
#include "event_loop.h"
#include "co_routine.h"
#include "tcp_client.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stack>
#include <unistd.h>
#include "sockets.h"

enum Status { kDisconnected, /* kDNSResolving, kDNSResolved, */ kConnecting, kConnected };

class Connector : public std::enable_shared_from_this<Connector>
{
    public:
        typedef std::function<void(int sockfd, const std::string& /*local addr*/)> NewConnectionCallback;
        Connector(EventLoop* loop, TCPClient *client) 
            : loop_(loop)
            , owner_tcp_client_(client) 
            , remote_addr_(client->remote_addr())
            , status_(kDisconnected)
        {
            //DLOG_TRACE << "raddr=" << remote_addr_;
            if (SplitHostPort(remote_addr_.data(), remote_host_, remote_port_)) {
                raddr_ = ParseFromIPPort(remote_addr_.data());
            }
            // create a coroutine
            ::co_create(&connect_co, NULL, HandleConnect, this);
            retry = false;
        }
        ~Connector()
        {
             //DLOG_TRACE;
            assert(loop_->IsInLoopThread());
            //if (status_ == kDNSResolving) {
            //    assert(!chan_.get());
            //    assert(!dns_resolver_.get());
            //    assert(!timer_.get());
            /*} else*/ 
            if (!IsConnected()) {
                // A connected tcp-connection's sockfd has been transfered to TCPConn.
                // But the sockfd of unconnected tcp-connections need to be closed by myself.
                //DLOG_TRACE << "close(" << chan_->fd() << ")";
                assert(own_fd_);
                assert(chan_->fd() == fd_);
                close(fd_);
                fd_ = -1;
            }
            assert(fd_ < 0);
            chan_.reset();
            fd_ = -1;
        }

        void Start()
        {
            //DLOG_TRACE << "Try to connect " << remote_addr_ << " status=" << StatusToString();
            assert(loop_->IsInLoopThread());
            if (!IsZeroAddress(&raddr_)) {
                Connect(); // In case users forgot to call Connect() first
            }
            co_resume(connect_co);       
        }

        void Cancel()
        {
        
        }

        void Connect()
        {
            //DLOG_TRACE << remote_addr_ << " status=" << StatusToString();
            assert(fd_ == -1);
            fd_ = CreateNonblockingSocket();
            own_fd_ = true;
            assert(fd_ >= 0);
            const std::string& laddr = owner_tcp_client_->local_addr();
            if (!laddr.empty()) {
                struct sockaddr_storage ss = ParseFromIPPort(laddr.data());
                struct sockaddr* addr = sockaddr_cast(&ss);
                int rc = ::bind(fd_, addr, sizeof(*addr));
                if (rc != 0) {
                    int serrno = errno;
                    LOG_ERROR << "bind failed, errno=" << serrno << " " << strerror(serrno);
                    HandleError();
                    return;
                }
            }
        }

    public:
        void SetNewConnectionCallback(NewConnectionCallback cb) 
        {
            conn_fn_ = cb;
        }
        bool IsConnecting() const 
        {
            return status_ == kConnecting;
        }
        bool IsConnected() const 
        {
            return status_ == kConnected;
        }
        bool IsDisconnected() const 
        {
            return status_ == kDisconnected;
        }
        int status() const 
        {
            return status_;
        }
    private:
        void SetStatus(Status status)
        {
            status_ = status;
        }
    private:
        static void* HandleConnect(void *);
        void HandleWrite()
        {
            //DLOG_TRACE << remote_addr_ << " status=" << StatusToString();
            if (status_ == kDisconnected) {
                // The connecting may be timeout, but the write event handler has been
                // dispatched in the EventLoop pending task queue, and next loop time the handle is invoked.
                // So we need to check the status whether it is at a kDisconnected
                //LOG_INFO << "fd=" << chan_->fd() << " remote_addr=" << remote_addr_ << " receive write event when socket is closed";
                return;
            }

            assert(status_ == kConnecting);
            int err = 0;
            socklen_t len = sizeof(len);
            if (getsockopt(chan_->fd(), SOL_SOCKET, SO_ERROR, (char*)&err, (socklen_t*)&len) != 0) {
                err = errno;
                //LOG_ERROR << "getsockopt failed err=" << err << " " << strerror(err);
            }

            if (err != 0) {
                //EVUTIL_SET_SOCKET_ERROR(err);
                HandleError();
                return;
            }

            assert(fd_ == chan_->fd());
            struct sockaddr_storage addr = sock::GetLocalAddr(chan_->fd());
            std::string laddr = sock::ToIPPort(&addr);
            conn_fn_(chan_->fd(), laddr);
            //timer_->Cancel();
            //timer_.reset();
            //chan_->DisableAllEvent();
            //chan_->Close();
            own_fd_ = false; // Move the ownership of the fd to TCPConn
            fd_ = -1;
            status_ = kConnected;
        }

        void SetFd(int fd)
        {
            fd_ = fd;
        }

        void HandleError()
        {
            //DLOG_TRACE << remote_addr_ << " status=" << StatusToString();
            assert(loop_->IsInLoopThread());
            int serrno = errno;
            // In this error handling method, we will invoke 'conn_fn_' callback function
            // to notify the user application layer in which the user maybe call TCPClient::Disconnect.
            // TCPClient::Disconnect may cause this Connector object desctruct.
            auto self = shared_from_this();
            //LOG_ERROR << "this=" << this << " status=" << StatusToString() << " fd=" << fd_  << " use_count=" << self.use_count() << " errno=" << serrno << " " << strerror(serrno);
            status_ = kDisconnected;
            //if (chan_) {
            //    assert(fd_ > 0);
            //    chan_->DisableAllEvent();
            //    chan_->Close();
            //}

            // Avoid DNSResolver callback again when timeout
            //if (dns_resolver_) {
            //    dns_resolver_->Cancel();
            //    dns_resolver_.reset();
            //}

            //timer_->Cancel();
            //timer_.reset();

            // If the connection is refused or it will not try again,
            // We need to notify the user layer that the connection established failed.
            // Otherwise we will try to do reconnection silently.
            //if (EVUTIL_ERR_CONNECT_REFUSED(serrno) || !owner_tcp_client_->auto_reconnect()) {
            //    conn_fn_(-1, "");
            //}

            // Although TCPClient has a Reconnect() method to deal with automatically reconnection problem,
            // TCPClient's Reconnect() will be invoked when a established connection is broken down.
            //
            // But if we could not connect to the remote server at the very beginning,
            // the TCPClient's Reconnect() will never be triggled.
            // So Connector needs to do reconnection automatically itself.
            if (owner_tcp_client_->auto_reconnect()) {
                // We must close(fd) firstly and then we can do the reconnection.
                if (fd_ > 0) {
                    //DLOG_TRACE << "Connector::HandleError close(" << fd_ << ")";
                    assert(own_fd_);
                    close(fd_);
                    fd_ = -1;
                }
                // Try to reconnect remote server.
                //DLOG_TRACE << "loop=" << loop_ << " auto reconnect in " << owner_tcp_client_->reconnect_interval().Seconds() << "s thread=" << std::this_thread::get_id();
                //loop_->RunAfter(owner_tcp_client_->reconnect_interval(), std::bind(&Connector::Start, shared_from_this()));
                retry = true;
            }
        }

        //void OnConnectTimeout();
        //void OnDNSResolved(const std::vector <struct in_addr>& addrs);
        std::string StatusToString() const
        {
            H_CASE_STRING_BIGIN(status_);
            H_CASE_STRING(kDisconnected);
            H_CASE_STRING(kDNSResolving);
            H_CASE_STRING(kDNSResolved);
            H_CASE_STRING(kConnecting);
            H_CASE_STRING(kConnected);
            H_CASE_STRING_END();
        }
    private:
        Status status_;
        EventLoop* loop_;
        TCPClient* owner_tcp_client_;
        struct stCoRoutine_t *connect_co;
        std::string remote_addr_; // host:port
        std::string remote_host_; // host
        int remote_port_ = 0; // port
        struct sockaddr_storage raddr_;
        //Duration timeout_;
        int fd_ = -1;
        bool retry;
        // A flag indicate whether the Connector owns this fd.
        // If the Connector owns this fd, the Connector has responsibility to close this fd.
        bool own_fd_ = false;
        std::unique_ptr<FdChannel> chan_; // ownership transfer
        //std::unique_ptr<TimerEventWatcher> timer_;
        //std::shared_ptr<DNSResolver> dns_resolver_;
        NewConnectionCallback conn_fn_;

    private:
        int GetFd() const
        {
            return fd_;
        }
        bool Auto_Retry() const
        {
            return retry; 
        }
        struct sockaddr_storage GetRemoteAddr() const
        {
            return raddr_;
        }
};

void* Connector::HandleConnect(void *connector)
{
    //co_enable_hook_sys();
    Connector *conn = (Connector *)connector;
    struct sockaddr_storage raddr = conn->GetRemoteAddr();
    struct sockaddr *addr = sockaddr_cast(&raddr);
    int fd = conn->GetFd();
    bool retry = conn->Auto_Retry(); 
	int ret = 0;

	for(;;) {
		if ( retry ) {
            fd = CreateNonblockingSocket();
        }
		ret = connect(fd, addr,sizeof(*addr));
		// EALREADY -- The socket is nonblocking and a previous connection attempt has not yet been completed.
		// EINPROGRESS -- The socket is nonblocking and the connection cannot be completed immediately.  
		//                It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  
		//                After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level 
		//                SOL_SOCKET  to  determine	whether connect()  completed	successfully  (SO_ERROR is zero) 
		//                or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason
		//   			  for the failure).
		if ( errno == EALREADY || errno == EINPROGRESS ) {       
			struct pollfd pf = { 0 };
			pf.fd = fd;
			pf.events = (POLLOUT|POLLERR|POLLHUP);
			co_poll( co_get_epoll_ct(),&pf,1,200); 
			//check connect
			int error = 0;
			uint32_t socklen = sizeof(error);
			errno = 0;
			ret = getsockopt(fd, SOL_SOCKET, SO_ERROR,(void *)&error,  &socklen);
			if ( ret == -1 ) {       
				//printf("getsockopt ERROR ret %d %d:%s\n", ret, errno, strerror(errno));
                //AddFailCnt();
                if (!retry) {
				    close(fd);
                    conn->SetFd(-1);
                    break;
                } else {
                    continue;
                }
			}       
			if ( error ) {       
			    errno = error;
			    //printf("connect ERROR ret %d %d:%s\n", error, errno, strerror(errno));
                //AddFailCnt();
                if (!retry) {
                    close(fd);
                    conn->SetFd(-1);
                    break; 
                } else {
                    continue;
                }
			}
		}
        conn->SetFd(fd);
        conn->SetStatus(kConnecting);
        break;
	}
}

#endif
