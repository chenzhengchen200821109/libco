#ifndef __LISTEN_H__
#define __LISTEN_H__

#include "inner_pre.h"
#include "co_routine.h"
#include "inetaddress.h"
#include <memory>
#include <stack>

class EventLoop;
class CoRoutine;
class TCPServer;

class Listener 
{
    public:
        Listener(EventLoop* loop, const InetAddress& listenAddr); 
        ~Listener();
	    void Listen(int backlog = SOMAXCONN);
        // nonblocking accept
        void Accept(); 
    private:
        //void SetAddr(const char *, const unsigned short shPort, struct sockaddr_in &addr);
        static void* HandleAcceptHelper(void *);
        void HandleAccept();
    private:
        EventLoop* loop_;
        TCPServer* owner_;
        std::unique_ptr<CoRoutine> PtrAcCo; // accept coroutine
        InetAddress listenAddr_;
        int fd_;
};

#endif
