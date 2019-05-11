#include "event_loop.h"
#include <functional>
#include <stack>

// Run the IO Event driving loop forever
// It MUST be called in the IO Event thread
void EventLoop::Run()
{
    status_.store(kRunning);
    for (size_t i = 0; i < coroutines_.size(); i++) {
        co_resume(coroutines_[i]);
    }
	co_eventloop(ctx, HandleEventLoop, this);
}

void EventLoop::Stop()
{
    status_.store(kStopped);
}

void EventLoop::RunAfter(int seconds, pFunc f) 
{
    struct stCoRoutine_t *co = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
    struct Argument *arg = (struct Argument *)malloc(sizeof(struct Argument));
    arg->seconds = seconds;
    arg->func = f;
    ::co_create(&co, NULL, HandleRunAfter, arg);
    QueueInLoop(co);
} 

void EventLoop::RunEvery(int seconds, pFunc f) 
{
    struct stCoRoutine_t *co = (struct stCoRoutine_t *)calloc(1, sizeof(struct stCoRoutine_t));
    struct Argument *arg = (struct Argument *)malloc(sizeof(struct Argument));
    arg->seconds = seconds;
    arg->func = f;
    ::co_create(&co, NULL, HandleRunEvery, arg);
    QueueInLoop(co);
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

int EventLoop::HandleEventLoop(void *loop)
{
    EventLoop *lp = (EventLoop *)loop;
    if (lp->IsStopped())
        return -1;
    else 
        return 0;
}

void* EventLoop::HandleRunAfter(void *arg)
{
    struct Argument *argument = (struct Argument *)arg;
    int seconds = argument->seconds;
    pFunc func = argument->func;
    free(argument);

    poll(NULL, 0, seconds * 1000);
    func();
    return 0;
}

void* EventLoop::HandleRunEvery(void *arg)
{
    struct Argument *argument = (struct Argument *)arg;
    int seconds = argument->seconds;
    pFunc func = argument->func;
    free(argument);

    for ( ; ; ) {
        poll(NULL, 0, seconds * 1000);
        func();
    }
}
