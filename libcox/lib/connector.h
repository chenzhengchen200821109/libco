#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "inner_pre.h"
#include <string>
#include <memory> 
#include <functional>
#include "coroutine.h"
#include "inetaddress.h"
#include "noncopyable.h"
#include "logging.h"
#include <assert.h>

class EventLoop;
class CoRoutine;

class Connector : public noncopyable 
{
    public:
        typedef std::function<void(int)> NewConnectionCallback;
        Connector(EventLoop* loop, const InetAddress& serverAddr); 
        ~Connector();
        void Start();
        //void Cancel();
        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback_ = cb;
        } 
        CoRoutine* GetCoRoutine() const
        {
            DLOG_TRACE;
            return co_;
        }
    private:
        static void* HandleConnect(void *arg);
        void Connect();
        void HandleWrite(int fd)
        {
            DLOG_TRACE;
            assert(fd >= 0);
            if (newConnectionCallback_)
                newConnectionCallback_(fd);
            else {
                DLOG_TRACE;
            }
        }
    private:
        EventLoop* loop_;
        InetAddress serverAddr_;
        std::unique_ptr<CoRoutine> PtrCo;
        CoRoutine* co_;
        NewConnectionCallback newConnectionCallback_;
};

#endif
