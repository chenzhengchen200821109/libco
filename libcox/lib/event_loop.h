#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "co_routine.h"
#include "co_routine_inner.h"
#include "inner_pre.h"
#include "server_status.h"
#include <stdlib.h>
#include <functional>
#include <stack>
#include <vector>
#include "coroutine.h"
#include "tcp_callbacks.h"


class EventLoop : public ServerStatus 
{
    public:
	    EventLoop() 
            : ctx(co_get_epoll_ct())
    	{
            DLOG_TRACE;
            tid_ = GetTid(); 
        }
        ~EventLoop()
        {
            DLOG_TRACE;
            //release coroutine 
        }
        void Run();
        void Stop();
        void RunAfter(int seconds, const Functor& f);
        void RunEvery(int seconds, const Functor& f);
        void QueueInLoop(std::unique_ptr<CoRoutine> PtrCo);
        //void QueueInLoop(struct CoRoutine* co)
        //{
        //    scoroutines_.push_back(co);
        //}
        const pid_t tid() const;
        bool IsInLoopThread() const;
    private:
        static int HandleEventLoopHelper(void *);
        int HandleEventLoop(); 
        static void *HandleRunAfter(void *);
        static void *HandleRunEvery(void *);
    private:
        struct stCoEpoll_t *ctx;
        std::vector<std::unique_ptr<CoRoutine> > coroutines_; // for TCP Clients
        //std::vector<struct CoRoutine *> scoroutines_; // for tcp server
        pid_t tid_;
        //std::stack<CoRoutine *> pool_; // for tcp server
};

#endif 
