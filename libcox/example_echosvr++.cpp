#include "co_routine.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <stack>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "event_loop.h"
#include "listener.h"

struct task_t*;

//
static void *readwrite_routine( void *arg )
{

	co_enable_hook_sys();

	task_t *co = (task_t*)arg;
	char buf[ 1024 * 16 ];
	for(;;)
	{
		if( -1 == co->fd )
		{
			g_readwrite.push( co );
			co_yield_ct();
			continue;
		}

		int fd = co->fd;
		co->fd = -1;

		for(;;)
		{
			struct pollfd pf = { 0 };
			pf.fd = fd;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll( co_get_epoll_ct(),&pf,1,1000); // 切换出去

			int ret = read( fd,buf,sizeof(buf) );
			if( ret > 0 )
			{
                buf[ret+1] = '\0';
                printf("%s\n", buf);
				ret = write( fd,buf,ret );
			}
			if( ret <= 0 )
			{
				// accept_routine->SetNonBlock(fd) cause EAGAIN, we should continue
				if (errno == EAGAIN)
					continue;
				close( fd );
				break;
			}
		}

	}
	return 0;
}

int main(int argc,char *argv[])
{
	if(argc<5){
		printf("Usage:\n"
               "example_echosvr [IP] [PORT] [TASK_COUNT] [PROCESS_COUNT]\n"
               "example_echosvr [IP] [PORT] [TASK_COUNT] [PROCESS_COUNT] -d   # daemonize mode\n");
		return -1;
	}
	const char *ip = argv[1];
	int port = atoi( argv[2] );
	int cnt = atoi( argv[3] ); //协程数量
	int proccnt = atoi( argv[4] ); //进程数量
	bool deamonize = argc >= 6 && strcmp(argv[5], "-d") == 0;

    //return a sockfd
	//g_listen_fd = CreateTcpSocket( port,ip,true );
	//listen( g_listen_fd,1024 );
	// g_listen_fd的判断应该在listen()函数调用之前吗？
	//if(g_listen_fd==-1){
	// 	printf("Port %d is in use\n", port);
    //		return -1;
	//}
    //listen(g_listen_fd, 1024);
	//printf("listen %d %s:%d\n",g_listen_fd,ip,port);

	//SetNonBlock( g_listen_fd );
    //
    EventLoop loop(0, 0);
    Listener listener(&loop, "");

	for(int k=0;k<proccnt;k++)
	{

		pid_t pid = fork();
		if( pid > 0 ) // in parent process
		{
			continue;
		}
		else if( pid < 0 )
		{
			break;
		}
		// in child process
		for(int i=0;i<cnt;i++)
		{
            // 
			task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
			task->fd = -1;
            // create I/O coroutines 
			co_create( &(task->co),NULL,readwrite_routine,task );
			co_resume( task->co );

		}

        // create listen coroutine
		//stCoRoutine_t *accept_co = NULL;
        /* create a coroutine who handles accept request */
		//co_create( &accept_co,NULL,accept_routine,0 );
		//co_resume( accept_co );
        listener.Listen(10, port, ip, true);
        listener.Accept();

		//co_eventloop( co_get_epoll_ct(),0,0 );
        loop.Run();

		exit(0);
	}
	if(!deamonize) wait(NULL);
	return 0;
}
/* ./example_echosvr 127.0.0.1 10000 100 50 */
