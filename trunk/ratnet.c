#include "ratnet.h"
#include <stdio.h>		// printf

#if defined(RATNET_WIN32)
extern struct RNET_eventop win32_eventop;
#elif defined(RATNET_LINUX)

#elif defined(RATNET_FREEBSD)

#endif

// event op
static struct RNET_eventop *my_eventop =
#if defined(RATNET_WIN32)
	&win32_eventop;
#elif defined(RATNET_LINUX)
#elif defined(RATNET_FREEBSD)
#endif



#define	MAX_EVENTS		20

struct RNET_event_base {
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

	int nfds;
	struct RNET_event evlist[MAX_EVENTS];
};

static struct RNET_event_base evbase;




// -----------------------------------------------------------------
// event stuff

void RNET_event_set(struct RNET_event *ev, RNET_socket fd, int events, event_callback func, void *args)
{
	ev->fd     = fd;
	ev->events = events;
	ev->func   = func;
	ev->args   = args;
}

void RNET_event_add(struct RNET_event *ev)
{
	int i = evbase.nfds;
	memcpy(&evbase.evlist[i], ev, sizeof(*ev));

	evbase.nfds++;

	if ( ev->events & EV_READ )
		FD_SET(ev->fd, &evbase.readfds);

	if ( ev->events & EV_WRITE )
		FD_SET(ev->fd, &evbase.writefds);
}

void RNET_event_loop(const struct timeval *tv)
{
	int i, n;
	fd_set readfds, writefds;
	readfds  = evbase.readfds;
	writefds = evbase.writefds;

	n = select(0, &readfds, &writefds, NULL, tv);
	if ( n == SOCKET_ERROR )
		RNET_errx("select() fail!");

	printf("select n = %d\n", n);
	if ( n == 0 )
		return;		// timeout
	
	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		struct RNET_event *ev = &evbase.evlist[i];

		if ( ev->events && (ev->events & EV_READ) && FD_ISSET(ev->fd, &readfds) )
			ev->func(ev->fd, EV_READ, ev->args);

		if ( ev->events && (ev->events & EV_WRITE) && FD_ISSET(ev->fd, &writefds) )
			ev->func(ev->fd, EV_WRITE, ev->args);
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
	in_addr.sin_addr.s_addr = inet_addr(addr);
	in_addr.sin_port        = htons(port);

	if ( bind(fd, (struct sockaddr *) &in_addr, sizeof(in_addr)) == SOCKET_ERROR )
		return SOCKET_ERROR;

	if ( listen(fd, 5) == SOCKET_ERROR )
		return SOCKET_ERROR;

	// set non-block
//	if ( my_eventop->setnonblock(fd) == SOCKET_ERROR )
//		return SOCKET_ERROR;

	return 0;
}

RNET_socket RNET_accept(RNET_socket listen_fd)
{
	return accept(listen_fd, NULL, NULL);
}

