#include "ratnet.h"
#include "ratnet_win32.h"
#include <assert.h>

#ifdef RATNET_WIN32

// -----------------------------------------------------------------
// global vars

#define	MAX_EVENTS		512

struct RNET_event_base {
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

	int fdcount;
	int max_fd;

	struct RNET_event* evlist[MAX_EVENTS];
};

static struct RNET_event_base evbase;



// -----------------------------------------------------------------
// event op

static void win32_init(void)
{
	WORD ver_request;
	WSADATA wsa_data;
	int err;

	ver_request = MAKEWORD(2, 2);

	err = WSAStartup( ver_request, &wsa_data );
	if ( err != 0 )
		RNET_errx("WSAStartup() failed!");
}

static void win32_shutdown(void)
{
	WSACleanup();
}

static int win32_setnonblock(RNET_socket fd)
{
	int mode = 1;
	return ioctlsocket(fd, FIONBIO, (u_long FAR*) &mode);
}

static int win32_addevent(struct RNET_event *ev)
{
	int i;

	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		if ( evbase.evlist[i] == NULL )
			break;
	}

	if ( i == MAX_EVENTS )
	{
		RNET_dbgmsg("evbase.evlist full");
		return SOCKET_ERROR;
	}

	ev->index = i;

	if ( ev->fd > evbase.max_fd ) evbase.max_fd = ev->fd;

	if ( ev->events & EV_READ )
		FD_SET(ev->fd, &evbase.readfds);

	if ( ev->events & EV_WRITE )
		FD_SET(ev->fd, &evbase.writefds);

	evbase.evlist[i] = ev;
	evbase.fdcount++;

	return 0;
}

static int win32_delevent(struct RNET_event *ev)
{
	ev->events &= (~EV_PERSIST);
	return 0;
}

static int _win32_delevent(struct RNET_event *ev)
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

	if ( events & EV_READ )
		FD_CLR(fd, &evbase.readfds);

	if ( events & EV_WRITE )
		FD_CLR(fd, &evbase.writefds);

	evbase.fdcount--;

	return 0;
}

static int win32_dispatch(int timeout)
{
	int i, n;
	int max_fd = 0;
	fd_set readfds, writefds;
	struct timeval tv = { timeout/1000, timeout%1000 };

	max_fd = evbase.max_fd;
	if ( max_fd == 0 ) return 0;
	if ( evbase.fdcount < 1 ) return 0;

	readfds  = evbase.readfds;
	writefds = evbase.writefds;

	n = select(max_fd+1, &readfds, &writefds, NULL, &tv);
	if ( n == SOCKET_ERROR )
	{
		// 如果系统中断发生不影响正常流程
		if (RNET_errno == RNET_EINTR) return 0;

		printf("select() error = %d\n", RNET_errno);
		return SOCKET_ERROR;
	}

	// printf("select n = %d, max_fd = %d\n", n, max_fd);
	if ( n == 0 )
		return 0;		// timeout
	
	for ( i = 0; i < MAX_EVENTS; i++ )
	{
		struct RNET_event *ev = evbase.evlist[i];
		if ( ev == NULL )
			continue;

		if ( (ev->events & EV_READ) && FD_ISSET(ev->fd, &readfds) )
		{
			ev->func(ev, ev->args);
			if ( (ev->events & EV_PERSIST) == 0 )
				_win32_delevent(ev);
			
			continue;
		}

		if ( (ev->events & EV_WRITE) && FD_ISSET(ev->fd, &writefds) )
		{
			ev->func(ev, ev->args);
			if ( (ev->events & EV_PERSIST) == 0 )
				_win32_delevent(ev);

			continue;
		}
	}

	return 0;	
}

struct RNET_eventop win32_eventop = {
	"win32_select",
	win32_init,
	win32_shutdown,
	win32_setnonblock,
	win32_addevent,
	win32_delevent,
	win32_dispatch,
};

#endif

