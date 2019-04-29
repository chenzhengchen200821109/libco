#ifndef __TCP_CALLBACKS_H__
#define __TCP_CALLBACKS_H__ 

#include <functional>
#include "inner_pre.h"
#include "buffer.h"
#include "tcp_conn.h"

//class Buffer;
//class TCPConn;

typedef std::shared_ptr<TCPConn> TCPConnPtr;
typedef std::function<void()> TimerCallback;

// When a connection established, broken down, connecting failed, this callback will be called
// This is called from a work-thread this is not the listening thread probably
typedef std::function<void(const TCPConnPtr&)> ConnectionCallback;
typedef std::function<void(const TCPConnPtr&)> CloseCallback;
typedef std::function<void(const TCPConnPtr&)> WriteCompleteCallback;
typedef std::function<void(const TCPConnPtr&, size_t)> HighWaterMarkCallback;
typedef std::function<void(const TCPConnPtr&, Buffer*)> MessageCallback;
inline void DefaultConnectionCallback(const TCPConnPtr&) {}
inline void DefaultMessageCallback(const TCPConnPtr&, Buffer*) {}

#endif
