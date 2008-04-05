#include "ratnet.h"
#include "ratnet_win32.h"

#ifdef RATNET_WIN32

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



struct RNET_eventop win32_eventop = {
	"win32",
	win32_init,
	win32_shutdown,
	win32_setnonblock,
};

#endif