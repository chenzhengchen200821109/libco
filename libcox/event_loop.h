#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "co_routine.h"
#include "co_routine_inner.h"
#include "inner_pre.h"
#include <stdlib.h>
#include <functional>
#include <stack>
#include <vector>

typedef int (*pEventFunc)(void *);

    class EventLoop {
        public:
            typedef std::function<void()> Functor;
	        EventLoop(pEventFunc func, void *arg) : func_(func), ctx(co_get_epoll_ct()), arg_(arg)
    	    {
                tid_ = GetTid(); 
            }

            //Build an EventLoop using an existing event_base object,
            // so we can embed an EventLoop object into the old applications based on libevent
            // NOTE: Be careful to deal with the destructing work of event_base_ and watcher_ objects.
            //explicit EventLoop(struct event_base);
            ~EventLoop()
            {
            
            }

            // Run the IO Event driving loop forever
            // It MUST be called in the IO Event thread
            void Run();

            void RunInLoop(const Functor& functor);

            void QueueInLoop(struct stCoRoutine_t* coroutine);

            const pid_t tid() const;

            bool IsInLoopThread() const;

            void SetAddr(struct sockaddr_in raddr)
            {
                raddr_ = raddr;
            }
            struct sockaddr_in GetAddr() const
            {
                return raddr_;
            }

            // Stop the event loop
            //void Stop();

            // Reinitialize some data fields after a fork
            //void AfterFork();

            //void RunAfter(double delay_ms, const Functor& f);

            // RunEvery executes Functor f every period interval time.
            //void RunEvery(Duration interval, const Functor& f);
        private:
            static void* HandleRunInLoop(void *arg);
            Functor GetNextFunctor();

        private:
            pEventFunc func_;
            struct stCoEpoll_t *ctx;
            void *arg_;
            std::stack<EventLoop::Functor> functors_;
            std::vector<struct stCoRoutine_t *> coroutines_;
            struct sockaddr_in raddr_;
            pid_t tid_;

    };

#endif 
