#include "ratnet.h"
#if defined(RATNET_FREEBSD)

#include "ratnet_freebsd.h"
#include <stdlib.h>

// ============================================================================
//                                                      Global Vars
// ============================================================================

#define	BACKLOG			128			// backlog for listen fd

#define	MAX_KQ_EVENTS		(1024 * 2)		// active events returned by kevent()
#define	MAX_EVENTS		(1024 * 20)		// 1024 * 20 == 2w events, enough for most app

// event type
#define	KQW_EVT_FREE		0			// event slot empty
#define	KQW_EVT_ACTIVE		1

static int kq;
static struct kevent kq_evlist[MAX_KQ_EVENTS];


// ============================================================================
//                                                      Kqueue Func
// ============================================================================

static void freebsd_init(void)
{
	if ( kq > 0 ) return;
	kq = kqueue();
	if ( kq == -1 ) RNET_errx("epoll_create() fail");
}

static void freebsd_shutdown(void)
{
	// do nth.
}

static int freebsd_setnonblock(RNET_socket fd)
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

static int freebsd_addevent(struct RNET_event *ev)
{
	int fd = ev->fd;
	struct kevent kev;
	int events = 0;

	if ( ev->events & EV_READ )  events |= EVFILT_READ;
	if ( ev->events & EV_WRITE ) events |= EVFILT_WRITE;

	EV_SET(&kev, fd, events, EV_ADD, 0, 0, ev);
	if ( kevent(kq, &kev, 1, NULL, 0, NULL) == -1 )
	{
		RNET_dbgmsg("kevent(add) fail");
		return SOCKET_ERROR;
	}

	return 0;
}

static int freebsd_delevent(struct RNET_event *ev)
{
	struct kevent kev;
	ev->events &= (~EV_PERSIST);

	EV_SET(&kev, ev->fd, EVFILT_READ|EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	return kevent(kq, &kev, 1, NULL, 0, NULL);
}

static int freebsd_dispatch(int timeout)
{ 
	struct RNET_event *ev;
	int i, n;

	n = kevent(kq, NULL, 0, &kq_evlist[0], MAX_KQ_EVENTS, NULL);
	if ( n < 0 && errno != EINTR )
		return -1;

	// boost events
	for ( i = 0; i < n; i++ )
	{
		ev = (struct RNET_event *) kq_evlist[i].udata;

		// TODO make event_callback ==> read/write_callback
		if ( ev->func )
		{
			ev->func(ev, ev->args);
			if ( (ev->events & EV_PERSIST) == 0 )
				free(ev);
		}
	}

	return 0;
}


// ============================================================================
//                                                      Export Event Op
// ============================================================================

struct RNET_eventop freebsd_eventop = {
	"freebsd_kqueue",
	freebsd_init,
	freebsd_shutdown,
	freebsd_setnonblock,
	freebsd_addevent,
	freebsd_delevent,
	freebsd_dispatch,
};

#endif
