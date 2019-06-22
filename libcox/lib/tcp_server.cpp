#include "inner_pre.h"
#include "event_loop.h"
#include "tcp_server.h"
#include "co_routine.h"
#include "listener.h"
#include <memory>

//std::stack<struct CoRoutine *> pool;

TCPServer::TCPServer(EventLoop* loop, const InetAddress& listenAddr, int num)
    : listener_(loop, listenAddr)
    , num_(num)
{
    DLOG_TRACE;

    //listener_(loop, this, ip, port, reuse);

    for (int i = 0; i < num_; i++)
    {
        //std::unique_ptr<CoRoutine> Co(new CoRoutine);
        //struct Argument* argu = (struct Argument *)calloc(1, sizeof(struct Argument));
        //argu->arg = this;
        //argu->co = Co.get();
        //::co_create(&(Co.get()->coroutine), NULL, HandleIO, argu);
        //struct CoRoutine* co = (struct CoRoutine *)calloc(1, sizeof(struct CoRoutine));
        //co->fd = -1;
        std::unique_ptr<CoRoutine> PtrCo(new CoRoutine);
        PtrCo.get()->owner = this;
        pool.push(PtrCo.get());
        ::co_create(&(PtrCo.get()->coroutine), NULL, HandleIOHelper, PtrCo.get());
        loop->QueueInLoop(std::move(PtrCo));
    }
    
}

TCPServer::~TCPServer()
{
    DLOG_TRACE;
}

void TCPServer::Start()
{
    DLOG_TRACE;
    listener_.Listen(1024); // listen loop
    listener_.Accept();
}

void* TCPServer::HandleIOHelper(void* arg)
{
    //TCPServer* server = (TCPServer *)arg;
    CoRoutine* co = (CoRoutine *)arg;
    TCPServer* server = (TCPServer *)(co->owner);
    server->HandleIO(co);
    return NULL;
}

void* TCPServer::HandleIO(void *arg)
{
    co_enable_hook_sys();

    struct CoRoutine* co = (struct CoRoutine *)arg;
    
	char buf[ 1024 * 16 ];

	for( ; ; ) 
    {
		if( -1 == co->fd )
		{
			pool.push( co );
			co_yield_ct();
			continue;
		}

		int fd = co->fd;
		co->fd = -1;

		for( ; ; ) {
			struct pollfd pf = { 0 };
			pf.fd = fd;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll(co_get_epoll_ct(), &pf, 1, 1000); // 切换出去

			int ret = read(fd, buf, sizeof(buf));
			if(ret > 0) {
				ret = write(fd, buf, ret);
			}
			if(ret <= 0)
			{
				// accept_routine->SetNonBlock(fd) cause EAGAIN, we should continue
				if (errno == EAGAIN)
					continue;
				close(fd);
				break;
			}
		}
	}
	return 0;
}


