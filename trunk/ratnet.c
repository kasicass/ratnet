// $Id: ratnet.c 832 2008-04-26 14:15:48Z kasicass $
#include "ratnet.h"
#include <stdio.h>		// printf
#include <stdlib.h>		// atexit
#include <string.h>		// memcpy, strcmp
#include <assert.h>


#if defined(RATNET_WIN32)
extern struct RNET_eventop win32_eventop;
#elif defined(RATNET_LINUX)
extern struct RNET_eventop linux_eventop;
#elif defined(RATNET_FREEBSD)

#endif

// event op
static struct RNET_eventop *my_eventop =
#if defined(RATNET_WIN32)
	&win32_eventop;
#elif defined(RATNET_LINUX)
	&linux_eventop;
#elif defined(RATNET_FREEBSD)
#endif



#define	MAX_EVENTS		20

struct RNET_event_base {
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

#if !defined(WIN32)
	int max_fd;
#endif
	struct RNET_event* evlist[MAX_EVENTS];
};

static struct RNET_event_base evbase;




// -----------------------------------------------------------------
// event stuff

struct RNET_event *
RNET_event_new()
{
	return (struct RNET_event *) malloc(sizeof(struct RNET_event));
}

void RNET_event_set(struct RNET_event *ev, RNET_socket fd, int events, event_callback func, void *args)
{
	ev->fd     = fd;
	ev->events = events;
	ev->func   = func;
	ev->args   = args;
	ev->index  = -1;
}

void RNET_event_add(struct RNET_event *ev)
{
	int i;

	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		if ( evbase.evlist[i] == NULL )
			break;
	}

	if ( i == MAX_EVENTS )
		RNET_errx("event_add() evlist full");

	ev->index = i;

#if !defined(WIN32)
	if ( ev->fd > evbase.max_fd ) evbase.max_fd = ev->fd;
#endif

	if ( ev->events & EV_READ )
		FD_SET(ev->fd, &evbase.readfds);

	if ( ev->events & EV_WRITE )
		FD_SET(ev->fd, &evbase.writefds);

	evbase.evlist[i] = ev;
}

void RNET_event_del(struct RNET_event *ev)
{
	int slot, events;
	RNET_socket fd;

	slot = ev->index;
	assert(slot != -1);

	fd     = ev->fd;
	events = ev->events;

	free(ev);	// free memory
	ev = NULL;

	evbase.evlist[slot] = NULL;

#if !defined(WIN32)
	if ( fd == evbase.max_fd )
	{
		int i;
		int max_fd = 0;
		for ( i = 0; i < MAX_EVENTS; i++ )
		{
			if ( evbase.evlist[i] == NULL )
				continue;

			if ( evbase.evlist[i]->fd > max_fd )
				max_fd = evbase.evlist[i]->fd;
		}
		evbase.max_fd = max_fd;
	}
#endif

	if ( events & EV_READ )
		FD_CLR(fd, &evbase.readfds);

	if ( events & EV_WRITE )
		FD_CLR(fd, &evbase.writefds);
}

void RNET_event_loop(struct timeval *tv)
{
	int i, n;
	int max_fd = 0;
	fd_set readfds, writefds;

#if !defined(WIN32)
	max_fd = evbase.max_fd;
	if ( max_fd == 0 ) return;
#endif

	readfds  = evbase.readfds;
	writefds = evbase.writefds;

	n = select(max_fd+1, &readfds, &writefds, NULL, tv);
	if ( n == SOCKET_ERROR )
		RNET_errx("select() fail!");

	// printf("select n = %d, max_fd = %d\n", n, max_fd);
	if ( n == 0 )
		return;		// timeout
	
	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		struct RNET_event *ev = evbase.evlist[i];
		if ( ev == NULL )
			continue;

		if ( (ev->events & EV_READ) && FD_ISSET(ev->fd, &readfds) )
		{
			ev->func(ev, ev->args);
			if ( (ev->events & EV_PERSIST) == 0 )
				RNET_event_del(ev);
			
			continue;
		}

		if ( (ev->events & EV_WRITE) && FD_ISSET(ev->fd, &writefds) )
		{
			ev->func(ev, ev->args);
			if ( (ev->events & EV_PERSIST) == 0 )
				RNET_event_del(ev);

			continue;
		}
	}
}



// -----------------------------------------------------------------
// network stuff

static void RNET_shutdown(void)
{
	my_eventop->shutdown();
	RNET_dbgmsg("network shutdown ...");
}

void RNET_init(void)
{
	my_eventop->init();
	atexit(RNET_shutdown);

	// init evbase
	RNET_bzero(&evbase, sizeof(evbase));
	FD_ZERO(&evbase.readfds);
	FD_ZERO(&evbase.writefds);
	FD_ZERO(&evbase.exceptfds);

	RNET_dbgmsg("network init ...");
}


RNET_socket RNET_create_tcp_socket()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int RNET_bind_and_listen(RNET_socket fd, const char* addr, int port)
{
	struct sockaddr_in in_addr;
	int reuseaddr_on = 1;

	// addr reuse
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseaddr_on,
                sizeof(reuseaddr_on)) == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	// bind & listen
	RNET_bzero(&in_addr, sizeof(in_addr));
	in_addr.sin_family      = AF_INET;
	in_addr.sin_addr.s_addr = (strcmp(addr, RNET_INADDR_ANY) == 0) ? htonl(INADDR_ANY) : inet_addr(addr);
	in_addr.sin_port        = htons(port);

	if ( bind(fd, (struct sockaddr *) &in_addr, sizeof(in_addr)) == SOCKET_ERROR )
		return SOCKET_ERROR;

	if ( listen(fd, 5) == SOCKET_ERROR )
		return SOCKET_ERROR;

	// set non-block
	if ( my_eventop->setnonblock(fd) == SOCKET_ERROR )
		return SOCKET_ERROR;

	return 0;
}

RNET_socket RNET_accept(RNET_socket listen_fd)
{
	return accept(listen_fd, NULL, NULL);
}

int RNET_connect(RNET_socket fd, const char* to_addr, int port)
{
	struct sockaddr_in addr;
	
	RNET_bzero(&addr, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = inet_addr(to_addr);
	addr.sin_port        = htons(port);

	if ( connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR )
	{
		if ( RNET_EAGAIN != RNET_errno )
			return SOCKET_ERROR;
	}

	return 0;
}


int RNET_sendbuffer(RNET_socket fd, struct RNET_buffer *buffer)
{
	return send(fd, RNET_buf_data(buffer), RNET_buf_len(buffer), 0);
}

int RNET_recvbuffer(RNET_socket fd, struct RNET_buffer *buffer)
{
	return recv(fd, RNET_buf_data(buffer), RNET_buf_size(buffer), 0);
}

