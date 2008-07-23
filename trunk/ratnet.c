// $Id: ratnet.c 4040 2008-07-19 14:53:57Z kasicass $
#include "ratnet.h"
#include <stdio.h>		// printf
#include <stdlib.h>		// atexit
#include <string.h>		// memcpy, strcmp
#include <assert.h>


// -----------------------------------------------------------------
// event op

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



// -----------------------------------------------------------------
// event stuff

struct RNET_event *
RNET_event_new()
{
	struct RNET_event *ev = (struct RNET_event *) malloc(sizeof(struct RNET_event));
	RNET_bzero(ev, sizeof(struct RNET_event));
	return ev;
}

void RNET_event_set(struct RNET_event *ev, RNET_socket fd, int events, event_callback func, void *args)
{
	ev->fd     = fd;
	ev->events = events;
	ev->func   = func;
	ev->args   = args;
}

void RNET_event_add(struct RNET_event *ev)
{
	my_eventop->addevent(ev);
}

void RNET_event_del(struct RNET_event *ev)
{
	my_eventop->delevent(ev);
}

void RNET_event_loop(int timeout)
{
	my_eventop->dispatch(timeout);
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

	printf("network init ... %s\n", my_eventop->name);
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
		RNET_errx("bind() fail");

	if ( listen(fd, 5) == SOCKET_ERROR )
		RNET_errx("listen() fail");

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

