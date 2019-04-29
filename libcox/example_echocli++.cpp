#include "co_routine.h"
#include "event_loop.h"
#include "connector.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

struct stEndPoint;
//{
//    char *ip;
//    unsigned short int port;
//};

int main(int argc,char *argv[])
{
	stEndPoint endpoint;
	endpoint.ip = argv[1];
	endpoint.port = atoi(argv[2]);
	int cnt = atoi( argv[3] ); //协程数量
	int proccnt = atoi( argv[4] ); //进程数量

	// 为什么要忽略SIGPIPE信号？
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, NULL );

    EventLoop loop(0, 0);
    //Connector conn(&loop, endpoint);

	for(int k=0;k<proccnt;k++)
	{

		pid_t pid = fork();
		if( pid > 0 ) // parent process
		{
			continue;
		}
		else if( pid < 0 )
		{
			break;
		}
		// child process
		for(int i=0;i<cnt;i++)
		{
			//stCoRoutine_t *co = 0;
			//co_create( &co,NULL,readwrite_routine, &endpoint);
			//co_resume( co );
            Connector conn(&loop, &endpoint);
            conn.Start();
		}
		co_eventloop( co_get_epoll_ct(),0,0 );

		exit(0);
	}
    
    // wait for all child process
    wait(NULL);
	return 0;
}
/*./example_echosvr 127.0.0.1 10000 100 50*/
