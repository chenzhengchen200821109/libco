#include "inner_pre.h"
#include "connector.h"
#include "event_loop.h"
#include "co_routine.h"
#include "sockets.h"
#include "utility.h"
#include <iostream>
#include <stdio.h>
#include <time.h>

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr) 
    : loop_(loop)
    , serverAddr_(serverAddr)
    , co_(0)
{
    DLOG_TRACE;
    PtrCo.reset(new CoRoutine);
    co_ = PtrCo.get();
    ::co_create(&(co_->coroutine), NULL, &Connector::HandleConnect, this);
}

Connector::~Connector()
{
    DLOG_TRACE;
    std::cout << "Connector::~Connector()" << std::endl;
}

void* Connector::HandleConnect(void* arg)
{
    LOG_TRACE;
    Connector* con = (Connector *)arg;
    con->Connect();
    return NULL;
}

void Connector::Connect()
{
    DLOG_TRACE;
    co_enable_hook_sys();

    int ret = 0;
    int fd = -1;

	for( ; ; ) {
        if (co_->fd < 0) {
            fd = sockets::CreateNonblockingSocket();
            ret = sockets::connect(fd, serverAddr_.getSockAddr());
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
                    continue;
			    }       
			    if (error) {       
			        errno = error;
                    close(fd);
                    fd = -1;
                    continue;
			    }
		    }
        }
        // connect succeed
        assert(fd >= 0 && ret == 0);
        co_->fd = fd;
        HandleWrite(fd);
    }
}

void Connector::Start()
{
    DLOG_TRACE;
    loop_->QueueInLoop(std::move(PtrCo));
}

