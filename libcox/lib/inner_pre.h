#include <assert.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <functional>
#include "sys_addrinfo.h"
#include "sys_sockets.h"
#include "sockets.h"
#include "logging.h"

typedef std::function<void()> Functor;
