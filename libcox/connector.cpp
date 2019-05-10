#include "inner_pre.h"
#include "connector.h"
#include "event_loop.h"
#include "co_routine.h"
#include "sockets.h"
#include "utility.h"
#include <iostream>
#include <stdio.h>
#include <time.h>

static int iSuccCnt;
static int iFailCnt;
static int iTime = 0;

void AddSuccCnt()
{
	int now = time(NULL);
	if (now >iTime)
	{
		printf("time %d Succ Cnt %d Fail Cnt %d\n", iTime, iSuccCnt, iFailCnt);
		iTime = now;
		iSuccCnt = 0;
		iFailCnt = 0;
	}
	else
	{
		iSuccCnt++;
	}
}

void AddFailCnt()
{
	int now = time(NULL);
	if (now >iTime)
	{
		printf("time %d Succ Cnt %d Fail Cnt %d\n", iTime, iSuccCnt, iFailCnt);
		iTime = now;
		iSuccCnt = 0;
		iFailCnt = 0;
	}
	else
	{
		iFailCnt++;
	}
}

Connector::Connector(EventLoop* loop, const struct stEndPoint* endpoint) 
    : loop_(loop)
{
    //DLOG_TRACE << "raddr=" << remote_addr_;
    std::cout << "Connector::Connector()" << std::endl;
    SetAddr(endpoint->ip, endpoint->port, raddr_);
    /* create a coroutine */
    ::co_create(&connect_co, NULL, HandleConnect, loop);
}

Connector::~Connector()
{
    //DLOG_TRACE;
    std::cout << "Connector::~Connector()" << std::endl;
}

void Connector::Start()
{
    //DLOG_TRACE << "Try to connect " << remote_addr_ << " status=" << StatusToString();
    std::cout << "Connector::Start()" << std::endl;
    loop_->SetAddr(raddr_);
    loop_->QueueInLoop(connect_co);
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

void* Connector::HandleConnect(void *loop)
{
    co_enable_hook_sys();
    EventLoop *lp = (EventLoop *)loop;
    struct sockaddr_in raddr = lp->GetAddr();
    char buf[1024 * 16];
    char str[8] = "sarlmol";
    int fd = -1;
    int ret = 0;

	for( ; ; ) {
        if (fd < 0) {
            fd = CreateNonblockingSocket();
            ret = connect(fd, (struct sockaddr *)&raddr, sizeof(raddr));
		    // EALREADY -- The socket is nonblocking and a previous connection attempt has not yet been completed.
		    // EINPROGRESS -- The socket is nonblocking and the connection cannot be completed immediately.  
		    //                It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  
		    //                After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level 
		    //                SOL_SOCKET  to  determine	whether connect()  completed	successfully  (SO_ERROR is zero) 
		    //                or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason
		    //   			  for the failure).
            perror("connect failed");
		    if (errno == EALREADY || errno == EINPROGRESS) {       
			    struct pollfd pf = { 0 };
			    pf.fd = fd;
			    pf.events = (POLLOUT|POLLERR|POLLHUP);
			    co_poll(co_get_epoll_ct(), &pf, 1, 200); 
			    //check connect
			    int error = 0;
			    uint32_t socklen = sizeof(error);
			    errno = 0;
			    ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &socklen);
			    if (ret == -1) {       
                    close(fd);
                    fd = -1;
                    AddFailCnt();
                    continue;
			    }       
			    if (error) {       
			        errno = error;
                    close(fd);
                    fd = -1;
                    AddFailCnt();
                    continue;
			    }
		    }
        }

        ret = write(fd, str, 8);
        if (ret > 0) {
            ret = read(fd, buf, sizeof(buf));
            if (ret <= 0) {
                close(fd);
                fd = -1;
                AddFailCnt();
            } else {
                AddSuccCnt();
            }
        } else {
            close(fd);
            fd = -1;
            AddFailCnt();
        }
	}
    return 0;
}

