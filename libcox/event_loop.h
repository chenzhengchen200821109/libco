#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "co_routine.h"
#include "co_routine_inner.h"
#include <stdlib.h>
#include <functional>
#include <stack>

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
            void Run()
	        {
		        co_eventloop(ctx, func_, arg_);
	        }

            void RunInLoop(const Functor& functor)
            {
                if (IsInLoopThread()) {
                    functors_.push(functor); 
                    struct stCoRoutine_t *co = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
                    co_create(&co, NULL, HandleRunInLoop, this);
                } else {
                    QueueInLoop(functor);
                }
            }

            void QueueInLoop(const Functor& functor)
            {
                functors_.push(functor);
            }

            const pid_t tid() const
            {
                return tid_;
            }

            bool IsInLoopThread() const
            {
                return tid_ == GetTid();
            }

            // Stop the event loop
            //void Stop();

            // Reinitialize some data fields after a fork
            //void AfterFork();

            //void RunAfter(double delay_ms, const Functor& f);

            // RunEvery executes Functor f every period interval time.
            //void RunEvery(Duration interval, const Functor& f);
        private:
            static void* HandleRunInLoop(void *arg)
            {
                EventLoop::Functor func;
                EventLoop *loop = (EventLoop *)arg;
                
                for ( ; ; ) {
                    func = loop->GetNextFunctor();
                    if (!func)
                       co_yield_ct();
                    else 
                        func(); 
                }

            }
            Functor GetNextFunctor()
            {
                Functor func;
                if (functors_.empty())
                    return func;
                else {
                    func = functors_.top();
                    functors_.pop();
                    return func;
                }
            }

        private:
            pEventFunc func_;
            struct stCoEpoll_t *ctx;
            void *arg_;
            std::stack<EventLoop::Functor> functors_;
            pid_t tid_;

    };

#endif 
