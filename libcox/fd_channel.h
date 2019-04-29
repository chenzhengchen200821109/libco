#ifndef __FD_CHANNEL_H__
#define __FD_CHANNEL_H__ 

#include <string>
#include "event_loop.h"

// A selectable I/O fd channel.
//
// This class doesn't own the file descriptor.
// The file descriptor could be a socket,
// an eventfd, a timerfd, or a signalfd
class FdChannel 
{
    public:
        enum EventType {
            kNone = 0x00,
            kReadable = 0x02,
            kWritable = 0x04,
        };
        typedef std::function<void()> WriteEventCallback;
        typedef std::function<void()> ReadEventCallback;

    public:
        FdChannel(EventLoop* loop, evpp_socket_t fd, bool watch_read_event, bool watch_write_event)
            : loop_(l), attached_(false), event_(nullptr), fd_(f) 
        {
            //DLOG_TRACE << "fd=" << fd_;
            assert(fd_ > 0);
            events_ = (r ? kReadable : 0) | (w ? kWritable : 0);
            event_ = new event;
             memset(event_, 0, sizeof(struct event));
        }

        ~FdChannel()
        {
            //DLOG_TRACE << "fd=" << fd_;
            assert(event_ == nullptr);
        }

        void Close()
        {
            //DLOG_TRACE << "fd=" << fd_;
            assert(event_);
            if (event_) {
                assert(!attached_);
                if (attached_) {
                    EventDel(event_);
                }
                delete (event_);
                event_ = nullptr;
            }
            read_fn_ = ReadEventCallback();
            write_fn_ = EventCallback();
        }

        // Attach this FdChannel to EventLoop
        void AttachToLoop()
        {
            assert(!IsNoneEvent());
            assert(loop_->IsInLoopThread());
            if (attached_) {
                // FdChannel::Update may be called many times 
                // So doing this can avoid event_add will be called more than once.
                DetachFromLoop();
            }
            assert(!attached_);
            ::event_set(event_, fd_, events_ | EV_PERSIST, &FdChannel::HandleEvent, this);
            ::event_base_set(loop_->event_base(), event_);
            if (EventAdd(event_, nullptr) == 0) {
                //DLOG_TRACE << "fd=" << fd_ << " watching event " << EventsToString();
                attached_ = true;
            } else {
                //LOG_ERROR << "this=" << this << " fd=" << fd_ << " with event " << EventsToString() << " attach to event loop failed";
            }
        }

        bool attached() const 
        {
            return attached_;
        }

    public:
        bool IsReadable() const 
        {
            return (events_ & kReadable) != 0;
        }
        bool IsWritable() const 
        {
            return (events_ & kWritable) != 0;
        }
        bool IsNoneEvent() const 
        {
            return events_ == kNone;
        }

        void EnableReadEvent()
        {
            int events = events_;
            events_ |= kReadable;
            if (events_ != events) {
                Update();
            }
        }

        void EnableWriteEvent()
        {
            int events = events_;
            events_ |= kWritable;
            if (events_ != events) {
                Update();
            }
        }

        void DisableReadEvent()
        {
            int events = events_;
            events_ &= (~kReadable);
            if (events_ != events) {
                Update();
            }
        }

        void DisableWriteEvent()
        {
            int events = events_;
            events_ &= (~kWritable);
            if (events_ != events) {
                Update();
            }
        }

        void DisableAllEvent()
        {
            if (events_ == kNone) {
                return;
            }
            events_ = kNone;
            Update();
        }

    public:
        int fd() const 
        {
            return fd_;
        }
        std::string EventsToString() const;

    public:
        void SetReadCallback(const ReadEventCallback& cb) 
        {
            read_fn_ = cb;
        }

        void SetWriteCallback(const EventCallback& cb) 
        {
            write_fn_ = cb;
        }

    private:
        void HandleEvent(evpp_socket_t fd, short which)
        {
            assert(sockfd == fd_);
            DLOG_TRACE << "fd=" << sockfd << " " << EventsToString();
            if ((which & kReadable) && read_fn_) {
                read_fn_();
            }

            if ((which & kWritable) && write_fn_) {
                write_fn_();
            }
        }

        static void HandleEvent(int fd, short which, void* v)
        {
            FdChannel* c = (FdChannel*)v;
            c->HandleEvent(sockfd, which);
        }

        void Update()
        {
            assert(loop_->IsInLoopThread());
            if (IsNoneEvent()) {
                DetachFromLoop();
            } else {
                AttachToLoop();
            }
        }

        void DetachFromLoop()
        {
            assert(loop_->IsInLoopThread());
            assert(attached_);
            if (EventDel(event_) == 0) {
                attached_ = false;
                //DLOG_TRACE << "fd=" << fd_ << " detach from event loop";
            } else {
                //LOG_ERROR << "DetachFromLoop this=" << this << "fd=" << fd_ << " with event " << EventsToString() << " detach from event loop failed";
            }
        }
    private:
        ReadEventCallback read_fn_;
        WriteEventCallback write_fn_;
        EventLoop* loop_;
        bool attached_; // A flag indicate whether this FdChannel has been attached to loop 
        struct event* event_;
        int events_; // the bitwise OR of zero or more of the EventType flags 
        int fd_;
};

#endif


