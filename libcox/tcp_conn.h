#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__

#include <atomic>
#include <string>
#include "inner_pre.h"
#include "buffer.h"
#include "tcp_callbacks.h"
#include "slice.h"
#include "event_loop.h"
//#include "any.h"
//#include "duration.h"

//class EventLoop;
//class FdChannel;
//class TCPClient;
//class InvokeTimer;

class TCPConn : public std::enable_shared_from_this<TCPConn> {
    public:
        enum Type 
        {
            kIncoming = 0, // The type of a TCPConn held by a TCPServer
            kOutgoing = 1, // The type of a TCPConn held by a TCPClient
        }   ;
        enum Status 
        {
            kDisconnected = 0,
            kConnecting = 1,
            kConnected = 2,
            kDisconnecting = 3,
        };
    public:
        TCPConn(EventLoop* loop,
            const std::string& name,
            int sockfd,
            const std::string& laddr,
            const std::string& raddr,
            uint64_t id);
            : loop_(loop)
            , fd_(sockfd)
            , id_(conn_id)
            , name_(n)
            , local_addr_(laddr)
            , remote_addr_(raddr)
            , status_(kDisconnected) 
        {
            if (sockfd >= 0) {
            //chan_.reset(new FdChannel(loop, sockfd, false, false));
            //chan_->SetReadCallback(std::bind(&TCPConn::HandleRead, this));
            //chan_->SetWriteCallback(std::bind(&TCPConn::HandleWrite, this));
            }

            //DLOG_TRACE << "TCPConn::[" << name_ << "] channel=" << chan_.get() << " fd=" << sockfd << " addr=" << AddrToString();
        }

        ~TCPConn() 
        {
            //DLOG_TRACE << "name=" << name()
            //    < " channel=" << chan_.get()
            //    << " fd=" << fd_ << " type=" << int(type())
            //    << " status=" << StatusToString() << " addr=" << AddrToString();
            assert(status_ == kDisconnected);

            if (fd_ >= 0) {
                //assert(chan_);
                //assert(fd_ == chan_->fd());
                //assert(chan_->IsNoneEvent());
                close(fd_);
                fd_ = -1;
            }

            //assert(!delay_close_timer_.get());
        }

        void TCPConn::Close() 
        {
            //DLOG_TRACE << "fd=" << fd_ << " status=" << StatusToString() << " addr=" << AddrToString();
            status_ = kDisconnecting;
            auto c = shared_from_this();
            auto f = [c]() {
                assert(c->loop_->IsInLoopThread());
                c->HnadleClose();
            };

            // Use QueueInLoop to fix TCPClient::Close bug when the application delete TCPClient in callback 
            loop_->QueueInLoop(f);
        }

        void Send(const char* s) 
        {
            Send(s, strlen(s));
        }

        void Send(const void* d, size_t dlen)
        {
            if (status_ != kConnected)
                return;
            else 
                SnedInLoop();
        
        }

	    void Send(const std::string& d) 
        {
            if (status_ != kConnected) {
                return;
            }

            if (loop_->IsInLoopThread()) {
                SendInLoop(d);
            } else {
                loop_->RunInLoop(std::bind(&TCPConn::SendStringInLoop, shared_from_this(), d));
            }
        }

        void Send(const Slice& message)
        {
            if (status_ != kConnected) {
                return;
            }
            if (loop_->IsInLoopThread()) {
                SendInLoop(message);
            } else {
                loop_->RunInLoop(std::bind(&TCPConn::SendStringInLoop, shared_from_this(), message.ToString()));
            }
        }

        void Send(Buffer* buf)
        {
            if (status_ != kConnected) {
                return;
            }
            if (loop_->IsInLoopThread()) {
                SendInLoop(buf->data(), buf->length());
                buf->Reset();
            } else {
                loop_->RunInLoop(std::bind(&TCPConn::SendStringInLoop, shared_from_this(), buf->NextAllString()));
            }
        
        }
    public:
        EventLoop* loop() const 
        {
            return loop_;
        }
        int fd() const 
        {
            return fd_;
        }
        uint64_t id() const 
        {
            return id_;
        }
        // Return the remote peer's address with form "ip:port"
        const std::string& remote_addr() const 
        {
            return remote_addr_;
        }
        const std::string& name() const 
        {
            return name_;
        }
        bool IsConnected() const 
        {
            return status_ == kConnected;
        }
        bool IsConnecting() const 
        {
            return status_ == kConnecting;
        }
        bool IsDisconnected() const 
        {
            return status_ == kDisconnected;
        }
        bool IsDisconnecting() const 
        {
            return status_ == kDisconnecting;
        }
        Type type() const 
        {
            return type_;
        }
        bool IsIncommingConn() const 
        {
            return type_ == kIncoming;
        }
        Status status() const 
        {
            return status_;
        }

        std::string AddrToString() const 
        {
            if (IsIncommingConn()) {
                return "(" + remote_addr_ + "->" + local_addr_ + "(local))";
            } else {
                return "(" + local_addr_ + "(local)->" + remote_addr_ + ")";
            }
        }

        // @brief When it is an incoming connection, we need to preserve the
        //  connection for a while so that we can reply to it. And we set a timer
        //  to close the connection eventually.
        // @param[in] d - If d.IsZero() == true, we will close the connection immediately.
        //void SetCloseDelayTime(Duration d) {
        //    assert(type_ == kIncoming);
        // This option is only available for the connection type kIncoming
        // Set the delay time to close the socket
        //close_delay_ = d;
        //}

    public:
        //void SetTCPNoDelay(bool on);
        // TODO Add : SetLinger();

        void ReserveInputBuffer(size_t len) { input_buffer_.Reserve(len); }
        void ReserveOutputBuffer(size_t len) { output_buffer_.Reserve(len); }

        void SetWriteCompleteCallback(const WriteCompleteCallback cb) 
        {
            write_complete_fn_ = cb;
        }

        //void SetHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t mark);
    protected:
        friend class TCPClient;
        friend class TCPServer;

        // These methods are visible only for TCPClient and TCPServer.
        // We don't want the user layer to access these methods.
        void set_type(Type t) 
        {
            type_ = t;
        }
        void SetMessageCallback(MessageCallback cb) 
        {
            msg_fn_ = cb;
        }
        void SetConnectionCallback(ConnectionCallback cb) 
        {
            conn_fn_ = cb;
        }
        void SetCloseCallback(CloseCallback cb) 
        {
            close_fn_ = cb;
        }
        //void OnAttachedToLoop();
        std::string StatusToString() const
        {
            H_CASE_STRING_BIGIN(status_.load());
            H_CASE_STRING(kDisconnected);
            H_CASE_STRING(kConnecting);
            H_CASE_STRING(kConnected);
            H_CASE_STRING(kDisconnecting);
            H_CASE_STRING_END();
        }
    private:
        void HandleRead()
        {
            assert(loop_->IsInLoopThread());
            int serrno = 0;
            ssize_t n = input_buffer_.ReadFromFD(chan_->fd(), &serrno);
            if (n > 0) {
                msg_fn_(shared_from_this(), &input_buffer_);
            } else if (n == 0) {
                if (type() == kOutgoing) {
                // This is an outgoing connection, we own it and it's done. so close it 
                //DLOG_TRACE << "fd=" << fd_ << ". We read 0 bytes and close the socket.";
                status_ = kDisconnecting;
                HandleClose();
                } else {
                    // Fix the half-closing problem : https://github.com/chenshuo/muduo/pull/117
                    //chan_->DisableReadEvent();
                    //if (close_delay_.IsZero()) {
                        //DLOG_TRACE << "channel (fd=" << chan_->fd() << ") DisableReadEvent. delay time " << close_delay_.Seconds() << "s. We close this connection immediately";
                        //DelayClose();
                    //} else {
                        // This is an incoming connection, we need to preserve the 
                        // connection for a while so that we can reply to it.
                        // And we set a timer to close the connection eventually.
                        //DLOG_TRACE << "channel (fd=" << chan_->fd() << ") DisableReadEvent. And set a timer to delay close this TCPConn, delay time " << close_delay_.Seconds() << "s"; 
                        //delay_close_timer_ = loop_->RunAfter(close_delay_, std::bind(&TCPConn::DelayClose, shared_from_this())); // TODO leave it to user layer close. 
                    }
                }
            } else {
                //if (EVUTIL_ERR_RW_RETRIABLE(serrno)) {
                    //DLOG_TRACE << "errno=" << serrno << " " << strerror(serrno);
                //} else {
                    //DLOG_TRACE << "errno=" << serrno << " " << strerror(serrno) << " We are closing this connection now.";
                    //HandleError();
                //}
            } 
        }

        void HandleWrite()
        {
            assert(loop_->IsInLoopThread());
            //assert(!chan_->attached() || chan_->IsWritable());
            ssize_t n = ::send(fd_, output_buffer_.data(), output_buffer_.length(), MSG_NOSIGNAL);
            if (n > 0) {
                output_buffer_.Next(n);
                if (output_buffer_.length() == 0) {
                    chan_->DisableWriteEvent();
                    if (write_complete_fn_) {
                        loop_->QueueInLoop(std::bind(write_complete_fn_, shared_from_this()));
                    }
                }
            } else {
                //int serrno = errno;
                //if (EVUTIL_ERR_RW_RETRIABLE(serrno)) {
                    //LOG_WARN << "this=" << this << " TCPConn::HandleWrite errno=" << serrno << " " << strerror(serrno);
                //} else {
                //    HandleError();
                //}
            } 
        }

        void HandleClose()
        {
            DLOG_TRACE << "addr=" << AddrToString() << " fd=" << fd_ << " status_=" << StatusToString();
            // Avoid multi calling
            if (status_ == kDisconnected) {
                return;
            }
            // We call HandleClose() from TCPConn's method, the status_ is kConnected 
            // But we call HandleClose() from out of TCPConn's method, the status_ is kDisconnecting 
            assert(status_ == kDisconnecting);
            status_ = kDisconnecting;
            assert(loop_->IsInLoopThread());
            //chan_->DisableAllEvent();
            //chan_->Close();
            TCPConnPtr conn(shared_from_this());
            //if (delay_close_timer_) {
                //DLOG_TRACE << "loop=" << loop_ << " Cancel the delay closing timer.";
                //delay_close_timer_->Cancel();
                //delay_close_timer_.reset();
            //}
            if (conn_fn_) {
                // This callback must be invoked at status kDisconnecting 
                // e.g. when the TCPClient disconnects with remote server,
                // the user layer can decide whether to do the reconnection.
                assert(status_ == kDisconnecting);
                conn_fn_(conn);
            }
            if (close_fn_) {
                close_fn_(conn);
            }
            //DLOG_TRACE << "addr=" << AddrToString() << " fd=" << fd_ << " status_=" << StatusToString() << " use_count=" << conn.use_count();
            status_ = kDisconnected; 
        }
        //void DelayClose();
        void HandleError()
        {
            DLOG_TRACE << "fd=" << fd_ << " status=" << StatusToString();
            status_ = kDisconnecting;
            HandleClose();
        }
        void SendInLoop(const Slice& message)
        {
            SendInLoop(message.data(), message.size());
        }
        void SendInLoop(const void* data, size_t len)
        {
            //assert(loop_->IsInLoopThread());
            if (status_ == kDisconnected) {
                //LOG_WARN << "disconnected, give up writing";
                return;
            }

            ssize_t nwritten = 0;
            size_t remaining = len;
            bool write_error = false;

            // if no data in output queue, writing directly
            if (!chan_->IsWritable() && output_buffer_.length() == 0) {
                while (remaining > 0) {
                    nwritten = ::send(chan_->fd(), static_cast<const char*>(data), remaining, MSG_NOSIGNAL);
                    if (nwritten >= 0) {
                        remaining = len - nwritten;
                        if (remaining == 0 && write_complete_fn_) {
                            loop_->QueueInLoop(std::bind(write_complete_fn_, shared_from_this()));
                        }
                    } else {
                        int serrno = errno;
                        nwritten = 0;
                        //if (!EVUTIL_ERR_RW_RETRIABLE(serrno)) {
                        //    LOG_ERROR << "SendInLoop write failed errno=" << serrno << " " << strerror(serrno);
                        //    if (serrno == EPIPE || serrno == ECONNRESET) {
                        //        write_error = true;
                        //    }
                        //}
                        close(cha_->fd());
                    }
                }    
            }

        }

    private:
        EventLoop* loop_;
        int fd_;
        uint64_t id_ = 0;
        std::string name_;
        std::string local_addr_; // the local address with form : "ip:port"
        std::string remote_addr_; // the remote address with form : "ip:port"
        Buffer input_buffer_;
        Buffer output_buffer_; // TODO use a list<Slice> ??

        std::atomic<Status> status_;


        ConnectionCallback conn_fn_; // This will be called to the user application layer
        MessageCallback msg_fn_; // This will be called to the user application layer
        WriteCompleteCallback write_complete_fn_; // This will be called to the user application layer
        HighWaterMarkCallback high_water_mark_fn_; // This will be called to the user application layer
        CloseCallback close_fn_; // This will be called to TCPClient or TCPServer
};

#endif
