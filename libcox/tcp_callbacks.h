#ifndef __TCP_CALLBACKS_H__
#define __TCP_CALLBACKS_H__ 

#include "inner_pre.h"

class Buffer;
class TCPConn;

typedef std::shared_ptr<TCPConn> TCPConnPtr;

// When a connection established, broken down, connecting failed, this callback will be called
// This is called from a work-thread this is not the listening thread probably
typedef std::function<void(const TCPConnPtr&)> ConnectionCallback;
typedef std::function<void(const TCPConnPtr&)> CloseCallback;
typedef std::function<void(const TCPConnPtr&)> WriteCompleteCallback;
typedef std::function<void(const TCPConnPtr&, Buffer *)> MessageCallback;
inline void DefaultConnectionCallback(const TCPConnPtr& conn) {}
inline void DefaultMessageCallback(const TCPConnPtr& conn, Buffer* buffer) {}

#endif
