#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "inner_pre.h"
#include <string>
#include <memory> 
#include <functional>
#include "coroutine.h"
#include <assert.h>

class EventLoop;

class Connector
{
    public:
        typedef std::function<void(int)> NewConnectionCallback;
        Connector(EventLoop* loop, const char* ip, const unsigned short port); 
        ~Connector();
        void Start();
        //void Cancel();
        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback_ = cb;
        } 
    public:
        Connector(Connector&& con) : loop_(con.loop_), ip_(con.ip_), port_(con.port_), PtrCo(std::move(con.PtrCo))
        {

        }
        Connector& operator=(Connector&& con)
        {
            if (this != &con)
                PtrCo = std::move(con.PtrCo);
            return *this;
        }
        Connector(const Connector&) = delete;
        Connector& operator=(const Connector&) = delete;
        // seter and geter are only used by TCPConn
        void SetFd(int fd)
        {
            fd_ = fd;
        }
        int GetFd() const
        {
            return fd_;
        }
    private:
        static void* HandleConnect(void *arg)
        {
            Connector *conn = (Connector *)arg;
            conn->Connect();
        }
        void Connect();
        void HandleWrite(int fd)
        {
            assert(fd >= 0);
            if (newConnectionCallback_)
                newConnectionCallback_(fd);
            else
                std::cout << "Connection Established" << std::endl;
        }
        void SetAddr(const char *, const unsigned short, struct sockaddr_in&);
    private:
        EventLoop* loop_;
        const char* ip_;
        const unsigned short port_;
        int fd_; // indicate ???
        std::unique_ptr<CoRoutine> PtrCo;
        struct sockaddr_in raddr_;
        NewConnectionCallback newConnectionCallback_;
};

#endif
