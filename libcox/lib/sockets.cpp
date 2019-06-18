#include "sockets.h"
#include "logging.h"
#include "sys_types.h"
#include "sys_endian.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string>

static const std::string empty_string;

std::string strerror(int errno) 
{
    char buf[2048] = {};
    const char* s = strerror_r(errno, buf, sizeof(buf) - 1); // GNU-specific
    if (s) {
        return std::string(s);
    }
    return empty_string;
}

int sockets::SetNonBlockingSocket(int fd)
{
    int flags;

	if ((flags = ::fcntl(fd, F_GETFL, NULL)) < 0) {
		return -1;
	}

    int ret = ::fcntl(fd, F_SETFL, flags | O_NDELAY | O_NONBLOCK);
    return ret;
}

int sockets::CreateNonblockingSocket() 
{
    int serrno = 0;

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        serrno = errno;
        LOG_ERROR << "socket error " << strerror(serrno);
        return -1;
    }

    if (SetNonBlockingSocket(fd) < 0) {
        goto out;
    }

    SetKeepAlive(fd, true);
    SetReuseAddr(fd);
    SetReusePort(fd);
    return fd;
out:
    close(fd);
    return -1;
}

void sockets::SetKeepAlive(int fd, bool on) 
{
    int optval = on ? 1 : 0;
    int rc = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                          reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    if (rc != 0) 
    {
        int serrno = errno;
        LOG_ERROR << "setsockopt(SO_KEEPALIVE) failed, errno=" << serrno << " " << strerror(serrno);
    }
}

void sockets::SetReuseAddr(int fd) 
{
    int optval = 1;
    int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                          reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    if (rc != 0) 
    {
        int serrno = errno;
        LOG_ERROR << "setsockopt(SO_REUSEADDR) failed, errno=" << serrno << " " << strerror(serrno);
    }
}

void sockets::SetReusePort(int fd) 
{
#ifdef SO_REUSEPORT
    int optval = 1;
    int rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT,
                          reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    if (rc != 0) 
    {
        int serrno = errno;
        LOG_ERROR << "setsockopt(SO_REUSEPORT) failed, errno=" << serrno << " " << strerror(serrno);
    }
#endif
}


void sockets::SetTCPNoDelay(int fd, bool on) 
{
    int optval = on ? 1 : 0;
    int rc = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                          reinterpret_cast<const char*>(&optval), static_cast<socklen_t>(sizeof optval));
    if (rc != 0) 
    {
        int serrno = errno;
        LOG_ERROR << "setsockopt(TCP_NODELAY) failed, errno=" << serrno << " " << strerror(serrno);
    }
}

int sockets::connect(int sockfd, const struct sockaddr* addr)
{
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr)
{
    return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr)
{
    return static_cast<const struct sockaddr_in6*>(implicit_cast<const void*>(addr));
}

void sockets::toIpPort(char* buf, size_t size,
                       const struct sockaddr* addr)
{
    toIp(buf,size, addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
    uint16_t port = networkToHost16(addr4->sin_port);
    assert(size > end);
    snprintf(buf+end, size-end, ":%u", port);
}

void sockets::toIp(char* buf, size_t size,
                   const struct sockaddr* addr)
{
    if (addr->sa_family == AF_INET)
    {
        assert(size >= INET_ADDRSTRLEN);
        const struct sockaddr_in* addr4 = sockaddr_in_cast(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    }
    else if (addr->sa_family == AF_INET6)
    {
        assert(size >= INET6_ADDRSTRLEN);
        const struct sockaddr_in6* addr6 = sockaddr_in6_cast(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
}

void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    {
        LOG_ERROR << "fromIpPort";
    }
}

void sockets::fromIpPort(const char* ip, uint16_t port,
                         struct sockaddr_in6* addr)
{
    addr->sin6_family = AF_INET6;
    addr->sin6_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET6, ip, &addr->sin6_addr) <= 0)
    {
        LOG_ERROR << "fromIpPort";
    }
}

int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in6 localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
    {
        LOG_ERROR << "getLocalAddr";
    }
    return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
    {
        LOG_ERROR << "getPeerAddr";
    }
    return peeraddr;
}

#if !(__GNUC_PREREQ (4,6))
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if (localaddr.sin6_family == AF_INET)
    {
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
        const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port
            && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else if (localaddr.sin6_family == AF_INET6)
    {
        return localaddr.sin6_port == peeraddr.sin6_port
            && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof localaddr.sin6_addr) == 0;
    }
    else
    {
        return false;
    }
}


