#include "inner_pre.h"
#include "connector.h"
#include "event_loop.h"
#include "co_routine.h"
#include "sockets.h"
#include "utility.h"
#include <iostream>
#include <stdio.h>
#include <time.h>

static int SetNonBlock(int iSock)
{
    int iFlags;

    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    int ret = fcntl(iSock, F_SETFL, iFlags);
    return ret;
}

Connector::Connector(EventLoop* loop, const char *ip, const unsigned short port) 
    : loop_(loop)
    , ip_(ip)
    , port_(port)
    , fd_(-1)
{
    //DLOG_TRACE << "raddr=" << remote_addr_;
    std::cout << "Connector::Connector()" << std::endl;
    PtrCo.reset(new CoRoutine);
    /* create a coroutine */
    //::co_create(&(PtrCo.get()->coroutine), NULL, HandleConnect, loop);
    ::co_create(&(PtrCo.get()->coroutine), NULL, HandleConnect, this);
}

Connector::~Connector()
{
    //DLOG_TRACE;
    std::cout << "Connector::~Connector()" << std::endl;
}

void Connector::Connect()
{
    co_enable_hook_sys();

    int ret = 0;

	for( ; ; ) {
        //printf("iSuccCnt = %d, iFailCnt = %d, iConnect = %d, iBytes = %d\n", iSuccCnt, iFailCnt, iConnect, iBytes);
        if (fd_ < 0) {
            fd_ = socket(AF_INET, SOCK_STREAM, 0);
            SetNonBlock(fd_);
            SetAddr(ip_, port_, raddr_);
            ret = connect(fd_, (struct sockaddr *)&raddr_, sizeof(raddr_));
		    // EALREADY -- The socket is nonblocking and a previous connection attempt has not yet been completed.
		    // EINPROGRESS -- The socket is nonblocking and the connection cannot be completed immediately.  
		    //                It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  
		    //                After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level 
		    //                SOL_SOCKET  to  determine	whether connect()  completed	successfully  (SO_ERROR is zero) 
		    //                or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason
		    //   			  for the failure).
            //perror("connect failed");
		    if (errno == EALREADY || errno == EINPROGRESS) {       
			    struct pollfd pf = { 0 };
			    pf.fd = fd_;
			    pf.events = (POLLOUT|POLLERR|POLLHUP);
			    co_poll(co_get_epoll_ct(), &pf, 1, 200); 
			    //check connect
			    int error = 0;
			    uint32_t socklen = sizeof(error);
			    errno = 0;
			    ret = getsockopt(fd_, SOL_SOCKET, SO_ERROR, (void *)&error, &socklen);
			    if (ret == -1) {       
                    close(fd_);
                    fd_ = -1;
                    continue;
			    }       
			    if (error) {       
			        errno = error;
                    close(fd_);
                    fd_ = -1;
                    continue;
			    }
		    }
        }
        // connect succeed
        assert(fd_ >= 0 && ret == 0);
        HandleWrite(fd_);
    }

}

void Connector::Start()
{
    //DLOG_TRACE << "Try to connect " << remote_addr_ << " status=" << StatusToString();
    std::cout << "Connector::Start()" << std::endl;
    loop_->QueueInLoop(std::move(PtrCo));
}

void Connector::SetAddr(const char *pszIP, const unsigned short shPort, struct sockaddr_in& addr)
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
