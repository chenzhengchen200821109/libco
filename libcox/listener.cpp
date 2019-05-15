#include "inner_pre.h"
#include "event_coroutine.h"
#include "event_loop.h"
#include "co_routine.h"
#include "sockets.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <functional>
#include <stdio.h>
#include <stack>
#include "listener.h"

void Listener::SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in &addr)
{
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(shPort);
	int nIP = 0;
	if( !pszIP || '\0' == *pszIP   
	    || 0 == strcmp(pszIP,"0") || 0 == strcmp(pszIP,"0.0.0.0") 
		|| 0 == strcmp(pszIP,"*") 
	  )
	{
		nIP = htonl(INADDR_ANY);
	}
	else
	{
		nIP = inet_addr(pszIP);
	}
	addr.sin_addr.s_addr = nIP;

}

Listener(EventLoop* loop,
        const char *ip, 
        const unsigned short port, 
        bool reuse) 
    : loop_(loop)
    , ip_(ip)
    , port_(port)
    , reuse_(reuse)
{
    ::co_create(&accept_co, NULL, HandleAccept, 0);
}

Listener::~Listener() 
{
    fd_ = -1;
}

void Listener::Listen(int backlog = SOMAXCONN)
{
    fd_ = CreateTcpSocket(port_, ip_, reuse_);
    if (fd_ == -1) {
        return;
    }
    listen(fd_, backlog);
    SetNonBlockingSocket(fd_);
}

        // nonblocking accept
void Listener::Accept() 
{
    loop_->QueueInLoop(accept_co);
}

extern int co_accept(int fd, struct sockaddr *addr, socklen_t *len);

void* Listener::HandleAccept(void *arg)
{
    co_enable_hook_sys();
	printf("accept_routine\n");
	fflush(stdout);
    
    Listener *lp = (Listener *)arg;
	for( ; ; ) {
		//printf("pid %ld g_readwrite.size %ld\n",getpid(),g_readwrite.size());
		if(lp->IsEmpty()) {// readwrite_routine协程还未准备好
			printf("empty\n"); //sleep
			struct pollfd pf = { 0 };
			pf.fd = -1;
			poll(&pf, 1, 1000); //切换出去
			continue;

		}
		struct sockaddr_in addr; //maybe sockaddr_un;
		memset(&addr, 0, sizeof(addr));
		socklen_t len = sizeof(addr);

		int fd = co_accept(g_listen_fd, (struct sockaddr *)&addr, &len);
		if( fd < 0 ) {
			struct pollfd pf = { 0 };
			pf.fd = g_listen_fd;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll(co_get_epoll_ct(), &pf, 1, 1000);
			continue;
		}
		if(lp->IsEmpty()) {// readwrite_routine协程还未准备好
			close( fd );
			continue;
		}
		SetNonBlockingSocket(fd); // fd现在是connected fd啦
		Task *co = lp->Top();
		co->fd = fd;
		lp->Pop();
		co_resume(co->co); // 执行readwrtie_routine协程
	}
	return 0;
}

