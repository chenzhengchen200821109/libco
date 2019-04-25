#include <assert.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <functional>
//#include "platform_config.h"
//#include "sys_addrinfo.h"
//#include "sys_sockets.h"
//#include "sockets.h"
//#include "logging.h"

struct event;
int EventAdd(struct event* ev, const struct timeval* timeout);
int EventDel(struct event*);
int GetActiveEventCount();
