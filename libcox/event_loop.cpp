#include "event_loop.h"
#include <functional>
#include <stack>

// Run the IO Event driving loop forever
// It MUST be called in the IO Event thread
void EventLoop::Run()
{
    for (size_t i = 0; i < coroutines_.size(); i++) {
        co_resume(coroutines_[i]);
    }
	co_eventloop(ctx, func_, arg_);
}

void EventLoop::RunInLoop(const Functor& functor)
{
    if (IsInLoopThread()) {
        functors_.push(functor); 
        struct stCoRoutine_t *co = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
        co_create(&co, NULL, HandleRunInLoop, this);
        co_resume(co);
    } 
}

void EventLoop::QueueInLoop(struct stCoRoutine_t *co)
{
    coroutines_.push_back(co);
}

const pid_t EventLoop::tid() const
{
    return tid_;
}

bool EventLoop::IsInLoopThread() const
{
    return tid_ == GetTid();
}

// Stop the event loop
//void Stop();

// Reinitialize some data fields after a fork
// void AfterFork();

//void RunAfter(double delay_ms, const Functor& f);

// RunEvery executes Functor f every period interval time.
//void RunEvery(Duration interval, const Functor& f);
void* EventLoop::HandleRunInLoop(void *arg)
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

EventLoop::Functor EventLoop::GetNextFunctor()
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


