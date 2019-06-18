#include "event_loop.h"
#include "connector.h"
#include "inetaddress.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string>

int main(int argc,char *argv[])
{
    //google::InitGoogleLogging(argv[0]);

    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddress serverAddr(argv[1], port);

    EventLoop loop;
    Connector con(&loop, serverAddr);

    con.Start();
    loop.Run();

    return 0;
}

