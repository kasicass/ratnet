#ifndef KCODE_RATNET_FREEBSD_H
#define KCODE_RATNET_FREEBSD_H
#ifdef RATNET_FREEBSD

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <sys/types.h>		// size_t
#include <sys/socket.h>
#include <sys/event.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>
#include <strings.h>		// bzero
#include <assert.h>
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
