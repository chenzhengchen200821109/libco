#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include <cstring>
#include <vector>
#include "inner_pre.h"
#include "event_loop.h"
#include "co_routine.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stack>
#include <unistd.h>
#include "sockets.h"

struct stEndPoint
{
	char *ip;
	unsigned short int port;
};

/* */
static void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in &addr)
{
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(shPort);
	int nIP = 0;
	if( !pszIP || '\0' == *pszIP
			|| 0 == std::strcmp(pszIP,"0") || 0 == std::strcmp(pszIP,"0.0.0.0")
			|| 0 == std::strcmp(pszIP,"*"))
	{
        /* convert from host byte to network byte order */
		nIP = htonl(INADDR_ANY);
	}
	else
	{
		nIP = inet_addr(pszIP);
	}
	addr.sin_addr.s_addr = nIP;

}

static int iSuccCnt = 0;
static int iFailCnt = 0;
static int iTime = 0;
static pid_t iPid = 0;

void AddSuccCnt()
{
    if (iPid == 0)
        iPid = getpid();
    /* returns the time as the number of seconds since the Epoch */
	int now = time(NULL);
	if (now >iTime)
	{
		printf("time %d PID[%d] TID[%d] Succ Cnt %d Fail Cnt %d\n", iTime, iPid, GetTid(), iSuccCnt, iFailCnt);
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
    if (iPid == 0)
        iPid = getpid();
	int now = time(NULL);
	if (now >iTime)
	{
		printf("time %d PID[%d] TID[%d] Succ Cnt %d Fail Cnt %d\n", iTime, iPid, GetTid(), iSuccCnt, iFailCnt);
		iTime = now;
		iSuccCnt = 0;
		iFailCnt = 0;
	}
	else
	{
		iFailCnt++;
	}
}

class Connector
{
    public:
        typedef std::function<void(int sockfd, const std::string& /*local addr*/)> NewConnectionCallback;
        //Connector(EventLoop* loop, const std::string& addr)
        Connector(EventLoop *loop, struct stEndPoint* endpoint)
           : loop_(loop), endpoint_(endpoint)
           // : loop_(loop), addr_(addr)
        {
            ::co_create(&connect_co, NULL, HandleConnect, this);
        }
        ~Connector()
        {
            fd_ = -1;
        }
        void Start()
        {
            co_resume(connect_co);       
        }
        struct stEndPoint* GetEndPoint() const
        {
            return endpoint_;
        }
        //void Cancel();
    public:
        void SetNewConnectionCallback(NewConnectionCallback cb) 
        {
            conn_fn_ = cb;
        }
        //bool IsConnecting() const 
        //{
        //    return status_ == kConnecting;
        //}
        //bool IsConnected() const 
        //{
        //    return status_ == kConnected;
        //}
        //bool IsDisconnected() const 
        //{
        //    return status_ == kDisconnected;
        //}
        //int status() const 
        //{
        //    return status_;
        //}
    private:
        static void* HandleConnect(void *);
        void HandleWrite();
        void SetFd(int fd)
        {
            fd_ = fd;
        }
        //void HandleError();
        //void OnConnectTimeout();
        //void OnDNSResolved(const std::vector <struct in_addr>& addrs);
        //std::string StatusToString() const;
    private:
        //enum Status { kDisconnected, kDNSResolving, kDNSResolved, kConnecting, kConnected };
        //Status status_;
        EventLoop* loop_;
        struct stCoRoutine_t *connect_co;
        struct stEndPoint* endpoint_;
        //std::string addr_;
        //TCPClient* owner_tcp_client_;

        //std::string remote_addr_; // host:port
        //std::string remote_host_; // host
        //int remote_port_ = 0; // port
        //struct sockaddr_storage raddr_;

        //Duration timeout_;

        int fd_ = -1;

        // A flag indicate whether the Connector owns this fd.
        // If the Connector owns this fd, the Connector has responsibility to close this fd.
        //bool own_fd_ = false;

        //std::unique_ptr<FdChannel> chan_;
        //std::unique_ptr<TimerEventWatcher> timer_;
        //std::shared_ptr<DNSResolver> dns_resolver_;
        NewConnectionCallback conn_fn_;
};

void* Connector::HandleConnect(void *connector)
{
    //co_enable_hook_sys();
    Connector *conn = (Connector *)connector;
	struct stEndPoint* endpoint = conn->GetEndPoint();
	char str[8]="sarlmol";
	char buf[ 1024 * 16 ];
	int fd = -1;
	int ret = 0;
	for(;;) {
		if ( fd < 0 ) {
            fd = CreateNonblockingSocket();
			//fd = socket(PF_INET, SOCK_STREAM, 0);
			struct sockaddr_in addr;
			SetAddr(endpoint->ip, endpoint->port, addr);
			ret = connect(fd,(struct sockaddr*)&addr,sizeof(addr));
			// EALREADY -- The socket is nonblocking and a previous connection attempt has not yet been completed.
			// EINPROGRESS -- The socket is nonblocking and the connection cannot be completed immediately.  
			//                It is possible to select(2) or poll(2) for completion by selecting the socket for writing.  
			//                After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level 
			//                SOL_SOCKET  to  determine	whether connect()  completed	successfully  (SO_ERROR is zero) 
			//                or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason
			//   			  for the failure).
			if ( errno == EALREADY || errno == EINPROGRESS ) {       
				struct pollfd pf = { 0 };
				pf.fd = fd;
				pf.events = (POLLOUT|POLLERR|POLLHUP);
				co_poll( co_get_epoll_ct(),&pf,1,200); 
				//check connect
				int error = 0;
				uint32_t socklen = sizeof(error);
				errno = 0;
				ret = getsockopt(fd, SOL_SOCKET, SO_ERROR,(void *)&error,  &socklen);
				if ( ret == -1 ) {       
					//printf("getsockopt ERROR ret %d %d:%s\n", ret, errno, strerror(errno));
					close(fd);
					fd = -1;
					AddFailCnt();
					continue;
				}       
				if ( error ) {       
					errno = error;
					//printf("connect ERROR ret %d %d:%s\n", error, errno, strerror(errno));
					close(fd);
					fd = -1;
					AddFailCnt();
					continue;
				}       
			} 
		}
        conn->SetFd(fd);
		
		ret = write( fd,str, 8);
		if ( ret > 0 ) {
			ret = read( fd,buf, sizeof(buf) );
			if ( ret <= 0 ) {
				//printf("co %p read ret %d errno %d (%s)\n",
				//		co_self(), ret,errno,strerror(errno));
				close(fd);
				fd = -1; // 有错误发生要重新建立连接
				AddFailCnt();
			} else {
                buf[ret+1] = '\0';
                printf("%s\n", buf);
				//printf("echo %s fd %d\n", buf,fd);
				AddSuccCnt();
			}
		} else {
			//printf("co %p write ret %d errno %d (%s)\n",
			//		co_self(), ret,errno,strerror(errno));
			close(fd);
			fd = -1; // 有错误发生要重新建立连接
			AddFailCnt();
		}
	}
}

#endif
