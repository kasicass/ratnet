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



// events type
#define	EV_READ				0x0001
#define	EV_WRITE			0x0002
#define	EV_PERSIST			0x0004

typedef void (*event_callback)(RNET_socket, int, void *);

struct RNET_event {
	RNET_socket		fd;
	int				events;
	event_callback	func;
	void *			args;
};


#define	RNET_INADDR_ANY		"RNET_inaddr_any"


// ------------------
// util func
void RNET_bzero(void *s, size_t n);

void RNET_errx(const char *msg);
void RNET_dbgmsg(const char *msg);


// ------------------
// event func
void RNET_event_set(struct RNET_event *ev, RNET_socket fd, int events, event_callback func, void *args);
void RNET_event_add(struct RNET_event *ev);
void RNET_event_loop(const struct timeval *tv);


// ------------------
// network func

// init network
void RNET_init(void);

// socket_fd      - ok
// INVALID_SOCKET - fail
RNET_socket RNET_create_tcp_socket();

// 0              - ok
// SOCKET_ERROR   - fail
int RNET_bind_and_listen(RNET_socket fd, const char* addr, int port);

// socket_fd      - ok
// SOCKET_ERROR   - fail
RNET_socket RNET_accept(RNET_socket listen_fd);

// 0              - ok
// SOCKET_ERROR   - fail
int RNET_connect(RNET_socket fd, const char *addr, int port);


#ifdef __cplusplus
}
#endif

#endif
