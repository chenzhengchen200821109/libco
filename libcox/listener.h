#ifndef __LISTEN_H__
#define __LISTEN_H__

#include "inner_pre.h"
#include "co_routine.h"
#include <stack>

class EventLoop;

class Listener 
{
    public:
        Listener(EventLoop* loop, const char *ip, const unsigned short port, bool reuse); 
        ~Listener();
	    void Listen(int backlog = SOMAXCONN);
        // nonblocking accept
        void Accept(); 
        struct Task
        {
            int fd;
            struct stCoRoutine_t *co;
        };
    private:
        bool IsEmpty()
        {
            return tasks.empty();
        }
        void Pop()
        {
            tasks.pop();
        }
        void Push(Task* task)
        {
            tasks.push(task);
        }
        Task* Top()
        {
            return tasks.top();
        }
    private:
        void SetAddr(const char *, const unsigned short shPort, struct sockaddr_in &addr);
        static void* HandleAccept(void *);
        EventLoop* loop_;
        struct stCoRoutine_t *accept_co;
        std::stack<Task *> tasks;
};

#endif
