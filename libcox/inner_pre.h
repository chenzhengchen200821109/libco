#include <assert.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <functional>
#include "sockets.h"

struct OnStartup
{
    OnStartup() { }
    ~OnStartup() { }
} __s_onstartup;

int EventAdd(struct event* ev, const struct timeval* timeout);
int EventDel(struct event*);
int GetActiveEventCount();
