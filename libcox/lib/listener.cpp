#include "event_coroutine.h"
#include "event_loop.h"
#include "co_routine.h"
#include "tcp_server.h"
#include "sockets.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <functional>
#include <stdio.h>
#include <stack>
#include "listener.h"

//extern std::stack<struct CoRoutine *> pool;

//void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in &addr);

//static int SetNonBlock(int iSock)
//{
//    int iFlags;

//    iFlags = fcntl(iSock, F_GETFL, 0);
//    iFlags |= O_NONBLOCK;
//    iFlags |= O_NDELAY;
//    int ret = fcntl(iSock, F_SETFL, iFlags);
//    return ret;
//}

//static int CreateTcpSocket(const unsigned short shPort /* = 0 */,const char *pszIP /* = "*" */,bool bReuse /* = false */)
//{
//	int fd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
//	if( fd >= 0 )
//	{
//		if(shPort != 0)
//		{
//			if(bReuse)
//			{
//				int nReuseAddr = 1;
//				setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&nReuseAddr,sizeof(nReuseAddr));
//			}
//			struct sockaddr_in addr ;
//			SetAddr(pszIP,shPort,addr);
//			int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
//			if( ret != 0)
//			{
//				close(fd);
//				return -1;
//			}
//		}
//	}
//	return fd;
//}

//void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in &addr)
//{
//	bzero(&addr,sizeof(addr));
//	addr.sin_family = AF_INET;
//	addr.sin_port = htons(shPort);
//	int nIP = 0;
//	if( !pszIP || '\0' == *pszIP   
//	    || 0 == strcmp(pszIP,"0") || 0 == strcmp(pszIP,"0.0.0.0") 
//		|| 0 == strcmp(pszIP,"*") 
//	  )
//	{
//		nIP = htonl(INADDR_ANY);
//	}
//	else
//	{
//		nIP = inet_addr(pszIP);
//	}
//	addr.sin_addr.s_addr = nIP;

//}

Listener::Listener(EventLoop* loop,const InetAddress& listenAddr)
    : loop_(loop)
    , listenAddr_(listenAddr)
{
    DLOG_TRACE;
    PtrAcCo.reset(new CoRoutine);
    ::co_create(&(PtrAcCo.get()->coroutine), NULL, HandleAcceptHelper, this);
}

Listener::~Listener() 
{
    DLOG_TRACE;
    fd_ = -1;
}

void Listener::Listen(int backlog)
{
    //fd_ = CreateTcpSocket(port_, ip_, reuse_);
    fd_ = sockets::CreateNonblockingSocket();
    if (fd_ == -1) {
        LOG_ERROR << "CreateTcpSocket failed";
        return;
    }
    sockets::bind(fd_, listenAddr_.getSockAddr());
    listen(fd_, backlog);
    //SetNonBlock(fd_);
}

// nonblocking accept
void Listener::Accept() 
{
    loop_->QueueInLoop(std::move(PtrAcCo));
}

extern int co_accept(int fd, struct sockaddr *addr, socklen_t *len);

void Listener::HandleAccept()
{
    DLOG_TRACE;
    co_enable_hook_sys();
	printf("accept_routine\n");
	fflush(stdout);
    
	for( ; ; ) {
		//printf("pid %ld g_readwrite.size %ld\n",getpid(),g_readwrite.size());
		if((owner_->pool).empty()) {// readwrite_routine协程还未准备好
            LOG_TRACE << "I/O coroutine pool is not ready";
			printf("empty\n"); //sleep
			struct pollfd pf = { 0 };
			pf.fd = -1;
			poll(&pf, 1, 1000); //切换出去
			continue;

		}
		struct sockaddr_in addr; //maybe sockaddr_un;
		memset(&addr, 0, sizeof(addr));
		socklen_t len = sizeof(addr);

		int fd = co_accept(fd_, (struct sockaddr *)&addr, &len);
		if( fd < 0 ) {
			struct pollfd pf = { 0 };
			pf.fd = fd_;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll(co_get_epoll_ct(), &pf, 1, 1000);
			continue;
		}
		if((owner_->pool).empty()) {// readwrite_routine协程还未准备好
			close( fd );
			continue;
		}
        sockets::SetNonBlockingSocket(fd); // fd现在是connected fd啦
		CoRoutine *co = (owner_->pool).top();
		co->fd = fd;
		(owner_->pool).pop();
		co_resume(co->coroutine); // 执行readwrtie_routine协程
	}
}

void* Listener::HandleAcceptHelper(void *arg)
{
    Listener *ls = (Listener *)arg;
    ls->HandleAccept();
    return NULL;
}

