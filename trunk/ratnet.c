#include "ratnet.h"

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


static void RNET_shutdown(void)
{
	my_eventop->shutdown();
	RNET_dbgmsg("network shutdown ...");
}

void RNET_init(void)
{
	my_eventop->init();
	atexit(RNET_shutdown);
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

