#ifndef CODEMATE_RATNET_H
#define CODEMATE_RATNET_H

#ifdef __cplusplus
extern "C" {
#endif

// platform switch
#define RATNET_WIN32
#undef RATNET_LINUX
#undef RATNET_LINUX


#if defined(RATNET_WIN32)
	#include "ratnet_win32.h"
#elif defined(RATNET_LINUX)
	#include "ratnet_linux.h"
#elif defined(RATNET_FREEBSD)
	#include "ratnet_freebsd.h"
#endif



/*
struct eventop {
	char *name;
	void *(*init)(struct event_base *);
	int (*add)(void *, struct event *);
	int (*del)(void *, struct event *);
	int (*recalc)(struct event_base *, void *, int);
	int (*dispatch)(struct event_base *, void *, struct timeval *);
	void (*dealloc)(struct event_base *, void *);
};
*/

struct RNET_eventop {
	const char *name;
	void (*init)(void);
	void (*shutdown)(void);
	int (*setnonblock)(RNET_socket fd);
};


// ------------------
// util func
void RNET_bzero(void *s, size_t n);

void RNET_errx(const char *msg);
void RNET_dbgmsg(const char *msg);


// ------------------
// main func

// init network
void RNET_init(void);

// socket_fd      - ok
// INVALID_SOCKET - fail
RNET_socket RNET_create_tcp_socket();

// 0              - ok
// SOCKET_ERROR   - fail
int RNET_bind_and_listen(RNET_socket fd, const char* addr, int port);


#ifdef __cplusplus
}
#endif

#endif
