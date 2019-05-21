#ifndef __LISTEN_H__
#define __LISTEN_H__

#include "inner_pre.h"
#include "co_routine.h"
#include <stack>

class EventLoop;
class CoRoutine;
class TCPServer;

class Listener 
{
    public:
        Listener(EventLoop* loop, const char *ip, const unsigned short port, bool reuse); 
        ~Listener();
	    void Listen(int backlog = SOMAXCONN);
        // nonblocking accept
        void Accept(); 
        //struct Task
        //{
        //    int fd;
        //    struct stCoRoutine_t *co;
        //};
    private:
       // bool IsEmpty()
       // {
       //     return pool.empty();
       // }
       // void Pop()
       // {
       //     pool.pop();
       // }
       // void Push(CoRoutine* co)
       // {
       //     pool.push(co);
       // }
       // CoRoutine* Top()
       // {
       //     return pool.top();
       // }
    private:
        void SetAddr(const char *, const unsigned short shPort, struct sockaddr_in &addr);
        static void* HandleAcceptHelper(void *);
        void HandleAccept();
    private:
        EventLoop* loop_;
        std::unique_ptr<CoRoutine> PtrAcCo; // accept coroutine
        //std::stack<CoRoutine *> pool; // coroutine pool
        const unsigned short port_;
        const char* ip_;
        bool reuse_;
        int fd_;
};

#endif
