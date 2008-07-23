#ifndef CODEMATE_RATNET_LINUX_H
#define CODEMATE_RATNET_LINUX_H
#ifdef RATNET_LINUX


#ifdef __cplusplus
extern "C" {
#endif

/* According to POSIX.1-2001 */
#include <sys/epoll.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>

#include <errno.h>

#define INVALID_SOCKET		(-1)
#define SOCKET_ERROR		(-1)

#define	RNET_socket		int
#define RNET_errno		errno

// error code
#define	RNET_EAGAIN		EAGAIN
#define	RNET_EINTR		EINTR

#ifdef __cplusplus
}
#endif


#endif
#endif

