#ifndef __LISTEN_H__
#define __LISTEN_H__

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

static void SetAddr(const char *pszIP,const unsigned short shPort,struct sockaddr_in &addr)
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

class Listener 
{
    public:
        typedef std::function <void(int fd, char *buf, len)> OnConnectionCallback;
        /* local listening address : IP:Port */
        Listener(EventLoop* loop, const std::string& addr) 
            : loop_(loop), addr_(addr)
        {
            //accept_co = new Coroutine(HandleAccept, 0); 
            ::co_create(&accept_co, NULL, HandleAccept, &fd_);
        }

        ~Listener() 
        {
            fd_ = -1;
        }

        // socket listen
        //void Listen(int backlog = SOMAXCONN)
	void Listen(int backlog = SOMAXCONN, const unsigned short shPort /* = 0 */,const char *pszIP /* = "*" */,bool bReuse /* = false */)
        {
            fd_ = CreateNonblockingSocket();
            if (fd_ < 0) {
                return;
            }
            //struct sockaddr_storage addr = ParseFromIPPort(addr_.data());
            // TODO Add retry when failed
            //int ret = ::bind(fd_, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof(struct sockaddr)));
            //if (ret < 0) {
                //int serrno = errno;
                //LOG_FATAL << "bind error :" << strerror(serrno) << " . addr=" << addr_;
            //}
	    if( fd >= 0 )
	{
		if(shPort != 0)
		{
			if(bReuse)
			{
				int nReuseAddr = 1;
				setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&nReuseAddr,sizeof(nReuseAddr));
			}
			struct sockaddr_in addr ;
			SetAddr(pszIP,shPort,addr);
			int ret = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
			if( ret != 0)
			{
				close(fd);
				return -1;
			}
		}
	}
             
            // sys call no hook
            ret = ::listen(fd_, backlog);
            if (ret < 0) {
                //int serrno = errno;
                //LOG_FATAL << "Listen failed " << strerror(serrno);
            }
        }

        // nonblocking accept
        void Accept() 
        {
            ::co_resume(accept_co);
        }

        //void Stop();

        // 
        void SetOnConnectionCallback(OnConnectionCallback cb) 
        {
            new_conn_fn_ = cb;
        }

    private:
        static void* HandleAccept(void *);
        int GetFd() const 
        {
            return fd_;
        }
        OnConnectionCallback GetOnConnectionCallback() const
        {
            return on_conn_fn_;
        }
        int fd_ = -1;// The listening socket fd
        EventLoop* loop_;
        //Coroutine *accept_co;
        struct stCoRoutine_t *accept_co;
        std::string addr_;
        //std::unique_ptr<FdChannel> chan_;
        OnConnectionCallback on_conn_fn_;
};

extern int co_accept(int fd, struct sockaddr *addr, socklen_t *len);
struct task_t 
{
    stCoRoutine_t *co;
    int fd;
};
extern stack<task_t *> g_readwrite;

void* Listener::HandleAccept(void *arg)
{
    Listener::OnConnectionCallback callback;
    Listener *listener = (Listener *)arg; 
    int fd = listener->GetFd();
    //printf("accept_routine\n");
    //fflush(stdout);

    // ??????
    for ( ; ; ) {
        //printf("pid %ld g_readwrite.size %ld\n",getpid(),g_readwrite.size());
		//if( g_readwrite.empty() ) {
		//	printf("empty\n"); //sleep
		//	struct pollfd pf = { 0 };
		//	pf.fd = -1;
            // poll 
		//	poll( &pf,1,1000); //切换出去
		//	continue;
		//}
		struct sockaddr_in addr; //maybe sockaddr_un;
		memset( &addr,0,sizeof(addr) );
		socklen_t len = sizeof(addr);

		int fd = co_accept(g_listen_fd, (struct sockaddr *)&addr, &len);
		if( fd < 0 ) {
			struct pollfd pf = { 0 };
			pf.fd = g_listen_fd;
			pf.events = (POLLIN|POLLERR|POLLHUP);
            // poll 
			co_poll( co_get_epoll_ct(),&pf,1,1000 );
			continue;
		}
        // call NewConnectionCallback when connection established
        callback = listener->GetNewConnectionCallback(); 
        // ?????
        callback(fd, );
        break;
		//if( g_readwrite.empty() ) { // readwrite_routine协程还未准备好
		//	close( fd );
		//	continue;
		//}
		//SetNonBlockingSocket( fd ); // fd现在是connected fd啦
		//task_t *co = g_readwrite.top();
		//co->fd = fd;
		//g_readwrite.pop();
		//co_resume( co->co ); // 执行readwrtie_routine协程
    }
    return NULL;
}

#endif
