#include "event_loop.h"
#include "inetaddress.h"
#include "tcp_client.h"
#include "tcp_conn.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string>

void onConnection(const TCPConnPtr& conn)
{
    conn->Send("world\n");
}

void onMessage(const TCPConnPtr& conn, Buffer *buf)
{
    std::string msg(buf->retrieveAllAsString());
    conn->Send(msg);
}

int main(int argc,char *argv[])
{
    //google::InitGoogleLogging(argv[0]);
    
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddress serverAddr(argv[1], port);

    int cocnt = atoi(argv[3]);
	int proccnt = atoi(argv[4]); //进程数量

	// 为什么要忽略SIGPIPE信号？
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, NULL );

    EventLoop loop;

	for(int k=0; k<proccnt; k++)
	{

		pid_t pid = fork();
		if( pid > 0 ) {
			continue;
		} else if( pid < 0 ) {
			break;
		}

        TCPClient cli(&loop, serverAddr, cocnt, "tcpclient");
        cli.setConnectionCallback(onConnection);
        cli.setMessageCallback(onMessage);
        cli.Start();

        loop.Run();

		exit(0);
	}
    
    // wait for all child process
    wait(NULL);
	return 0;
}
