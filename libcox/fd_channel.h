#ifndef __FD_CHANNEL_H__
#define __FD_CHANNEL_H__ 

class EventLoop;

// A selectable I/O fd channel.
//
// This class doesn't own the file descriptor.
// The file descriptor could be a socket,
// an eventfd, a timerfd, or a signalfd
class FdChannel 
{
    public:
        FdChannel(EventLoop* loop, int fd);
        ~FdChannel();
        //void Close();
        // Attach this FdChannel to EventLoop
        //void AttachToLoop();
        int fd() const 
        {
            return fd_;
        }
    private:
        EventLoop* loop_;
        int fd_;
};

#endif


