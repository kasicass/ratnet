#include "ratnet.h"
#include "ratnet_linux.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef RATNET_LINUX

// ---------- global vars -----------

// 对于 epoll 相关的变量，全部使用前缀 e
// 比如 epoll_event eev;

#define	MAX_EVENTS		1024

struct RNET_event_base {
	int efd;		// epoll fd
	struct epoll_event eev_list[MAX_EVENTS];
};

static struct RNET_event_base evbase;


// ---------- func -----------

static void linux_init(void)
{
	// 
	RNET_bzero(&evbase, sizeof(evbase));

	// init epoll
	evbase.efd = epoll_create(MAX_EVENTS);
	if ( evbase.efd == -1 )
		RNET_errx("epoll_create() fail");
}

static void linux_shutdown(void)
{
	close(evbase.efd);
}

static int linux_setnonblock(RNET_socket fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		goto label_nonblock_error;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		goto label_nonblock_error;

	return 0;

label_nonblock_error:
	return SOCKET_ERROR;
}

static int linux_addevent(struct RNET_event *ev)
{
	int events = 0;
	struct epoll_event eev;

	RNET_bzero(&eev, sizeof(eev));

	if ( ev->events & EV_READ )  events |= EPOLLIN;
	if ( ev->events & EV_WRITE ) events |= EPOLLOUT;
		
	eev.events   = events;
	eev.data.ptr = (void *) ev;
	if ( epoll_ctl(evbase.efd, EPOLL_CTL_ADD, ev->fd, &eev) == -1 )
	{
		RNET_dbgmsg("epoll_ctl(add) fail");
		return SOCKET_ERROR;
	}

	return 0;
}

static int linux_delevent(struct RNET_event *ev)
{
	ev->events &= (~EV_PERSIST);

	// remove from epoll
	if ( epoll_ctl(evbase.efd, EPOLL_CTL_DEL, ev->fd, NULL) == -1 )
	{
		RNET_dbgmsg("epoll_ctl(del) fail");
		return SOCKET_ERROR;
	}
	return 0;
}

static int _free_ev(struct RNET_event *ev)
{
	// epoll_ctl(del, ev->fd) 不能放在这里, 因为 ev->func() 在 RNET_event_del() 后, 很可能会
	// close(ev->fd), 则 epoll_ctl(del, ev->fd) 失败

	free(ev);
	return 0;
}

static int linux_dispatch(int timeout)
{
	int i, n;

	n = epoll_wait(evbase.efd, evbase.eev_list, MAX_EVENTS, timeout);
	if ( n == -1 )
	{
		if (errno == EINTR) return 0;

		printf("epoll_wait() fail: %d\n", errno);
		return SOCKET_ERROR;
	}
	else if ( n == 0 )
	{
		// timeout
		return 0;
	}

	// dispatch event
	for ( i = 0; i < n; i++ )
	{
		struct RNET_event *ev = (struct RNET_event *) evbase.eev_list[i].data.ptr;

		ev->func(ev, ev->args);
		if ( (ev->events & EV_PERSIST) == 0 )
			_free_ev(ev);
	}

	return 0;
}


struct RNET_eventop linux_eventop = {
	"linux_epoll",
	linux_init,
	linux_shutdown,
	linux_setnonblock,
	linux_addevent,
	linux_delevent,
	linux_dispatch,
};

#endif

