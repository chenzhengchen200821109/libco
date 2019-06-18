#include "event_loop.h"
#include <functional>

void EventLoop::Run()
{
    DLOG_TRACE;
    status_.store(kRunning);
    //for (size_t i = 0; i < coroutines_.size(); i++) {
    //    co_resume(coroutines_[i].get()->coroutine);
    //}
    //for (size_t i = 0; i < scoroutines_.size(); i++)
    //{
    //    co_resume(scoroutines_[i]->coroutine);
    //}
    for (size_t i = 0; i < coroutines_.size(); i++) {
        co_resume(coroutines_[i].get()->coroutine);
    }
	::co_eventloop(ctx, HandleEventLoopHelper, this);
}

void EventLoop::Stop()
{
    DLOG_TRACE;
    status_.store(kStopped);
}

void EventLoop::RunAfter(int seconds, const Functor& f) 
{
    DLOG_TRACE;
    std::unique_ptr<CoRoutine> PtrCo(new CoRoutine(f, seconds));
    ::co_create(&(PtrCo.get()->coroutine), NULL, HandleRunAfter, PtrCo.get());
    QueueInLoop(std::move(PtrCo));
} 

void EventLoop::RunEvery(int seconds, const Functor& f) 
{
    DLOG_TRACE;
    std::unique_ptr<CoRoutine> PtrCo(new CoRoutine(f, seconds));
    ::co_create(&(PtrCo.get()->coroutine), NULL, HandleRunEvery, PtrCo.get());
    QueueInLoop(std::move(PtrCo));
} 

void EventLoop::QueueInLoop(std::unique_ptr<CoRoutine> PtrCo)
{
    DLOG_TRACE;
    coroutines_.push_back(std::move(PtrCo));
}

const pid_t EventLoop::tid() const
{
    DLOG_TRACE;
    return tid_;
}

bool EventLoop::IsInLoopThread() const
{
    DLOG_TRACE;
    return tid_ == GetTid();
}

int EventLoop::HandleEventLoopHelper(void *loop)
{
    LOG_TRACE;
    EventLoop *lp = (EventLoop *)loop;
    return lp->HandleEventLoop();
}

int EventLoop::HandleEventLoop()
{
    DLOG_TRACE;
    if (IsStopped())
        return -1;
    else 
        return 0;
}

void* EventLoop::HandleRunAfter(void *arg)
{
    LOG_TRACE;
    CoRoutine *co = (CoRoutine *)arg;
    int seconds = co->seconds;
    Functor func = co->functor;

    poll(NULL, 0, seconds * 1000);
    func();
    return 0;
}

void* EventLoop::HandleRunEvery(void *arg)
{
    LOG_TRACE; 
    CoRoutine *co = (CoRoutine *)arg;
    int seconds = co->seconds;
    Functor func = co->functor;

    for ( ; ; ) 
    {
        poll(NULL, 0, seconds * 1000);
        func();
    }
    return 0;
}
