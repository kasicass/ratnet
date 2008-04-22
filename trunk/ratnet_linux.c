#include "ratnet.h"
#include "ratnet_linux.h"

#ifdef RATNET_LINUX


static void linux_init(void)
{
	// no-ops
}

static void linux_shutdown(void)
{
	// no-ops
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

struct RNET_eventop linux_eventop = {
	"linux",
	linux_init,
	linux_shutdown,
	linux_setnonblock,
};

#endif

