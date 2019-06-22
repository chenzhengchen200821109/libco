#include "tcp_server.h"
#include "event_loop.h"
#include "inetaddress.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
    //google::InitGoogleLogging(argv[0]);
	if(argc<5){
		printf("Usage:\n"
               "example_echosvr [IP] [PORT] [TASK_COUNT] [PROCESS_COUNT]\n"
               "example_echosvr [IP] [PORT] [TASK_COUNT] [PROCESS_COUNT] -d   # daemonize mode\n");
		return -1;
	}
	//const char *ip = argv[1];
	uint16_t port = static_cast<uint16_t>(atoi( argv[2] ));
	int cnt = atoi( argv[3] ); //协程数量
	int proccnt = atoi( argv[4] ); //进程数量
	bool deamonize = argc >= 6 && strcmp(argv[5], "-d") == 0;
    InetAddress listenAddr(argv[1], port);

    EventLoop loop;

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

        TCPServer server(&loop, listenAddr, cnt);
        server.Start();
        loop.Run();
	    exit(0);
	}

	if(!deamonize) wait(NULL);
	return 0;
}
