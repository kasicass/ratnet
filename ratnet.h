#ifndef KCODE_RATNET_CORE_H
#define KCODE_RATNET_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ratnet_platform_detect.h"
#include "ratnet_buffer.h"


#define	RNET_INADDR_ANY		"RNET_inaddr_any"



// ------------- base type -------------

// events type
#define	EV_READ				0x0001
#define	EV_WRITE			0x0002
#define	EV_PERSIST			0x0004

struct RNET_event;
typedef void (*event_callback)(struct RNET_event*, void *);

struct RNET_event {
	RNET_socket	fd;
	int		events;
	event_callback	func;
	void *		args;

#ifdef RATNET_WIN32
	int		index;
#endif
};


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

	// 0            - ok		// 所有的返回都如此
	// SOCKET_ERROR - fail
	int (*setnonblock)(RNET_socket fd);

	int (*addevent)(struct RNET_event *);
	int (*delevent)(struct RNET_event *);

	int (*dispatch)(int timeout);	// timeout - millisecond
};


// ------------------
// util func
void RNET_bzero(void *s, size_t n);

void RNET_errx(const char *msg);
void RNET_dbgmsg(const char *msg);


// ------------------
// event func
struct RNET_event *RNET_event_new();
void RNET_event_set(struct RNET_event *ev, RNET_socket fd, int events, event_callback func, void *args);
void RNET_event_add(struct RNET_event *ev);
void RNET_event_del(struct RNET_event *ev);
void RNET_event_loop(int timeout);


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


// ------------------
// send/recv func

// nbytes sent    - ok
// SOCKET_ERROR   - fail
int RNET_sendbuffer(RNET_socket fd, struct RNET_buffer *buffer);

// nbytes recv    - ok
// SOCKET_ERROR   - fail
int RNET_recvbuffer(RNET_socket fd, struct RNET_buffer *buffer);


#ifdef __cplusplus
}
#endif

#endif
