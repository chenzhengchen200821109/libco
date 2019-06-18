#ifndef __SOCKETS_H__
#define __SOCKETS_H__

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // for TCP_NODELAY
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

namespace sockets
{

int SetNonBlockingSocket(int sockfd);
int CreateNonblockingSocket();
void SetKeepAlive(int sockfd, bool on);
void SetReuseAddr(int sockfd);
void SetReusePort(int sockfd);
void SetTCPNoDelay(int sockfd, bool on);

int connect(int sockfd, const struct sockaddr* addr);

void toIpPort(char* buf, size_t size,
              const struct sockaddr* addr);
void toIp(char* buf, size_t size,
          const struct sockaddr* addr);
void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in* addr);
void fromIpPort(const char* ip, uint16_t port,
                struct sockaddr_in6* addr);
int getSocketError(int sockfd);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
struct sockaddr_in6 getLocalAddr(int sockfd);
struct sockaddr_in6 getPeerAddr(int sockfd);
bool isSelfConnect(int sockfd);

} //namespace sockets

#endif
