#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "inner_pre.h"
#include <string>

class EventLoop;

struct stEndPoint
{
    char *ip;
    unsigned short int port;
};

class Connector
{
    public:
        Connector(EventLoop* loop, const struct stEndPoint* endpoint); 
        ~Connector();
        void Start();
    private:
        static void* HandleConnect(void *);
        void SetAddr(const char *, const unsigned short, struct sockaddr_in&);
    private:
        EventLoop* loop_;
        struct stCoRoutine_t *connect_co;
        struct sockaddr_in raddr_;
};

#endif
