#include "ratnet.h"
#include <stdio.h>

#if defined(_DEBUG)
#pragma comment(lib, "..\\debug\\ratnet_d.lib")
#else
#pragma comment(lib, "..\\release\\ratnet.lib")
#endif


void on_write(RNET_socket fd, int ev, void *arg)
{
	puts("send ...");
	send(fd, "abc", 3, 0);

	Sleep(1000);
}


int main(void)
{
	RNET_socket sock_fd;
	struct RNET_event   ev_write;


	RNET_init();

	sock_fd = RNET_create_tcp_socket();
	if ( INVALID_SOCKET == sock_fd )
		RNET_errx("RNET_create_tcp_socket() fail!");

	if ( RNET_connect(sock_fd, "192.168.1.36", 8888) == SOCKET_ERROR )
		RNET_errx("RNET_connect() failed!");

	puts("connect ok");


	RNET_event_set(&ev_write, sock_fd, EV_WRITE|EV_PERSIST, on_write, NULL);
	RNET_event_add(&ev_write);

	while (1)
	{
		RNET_event_loop(NULL);
	}

	return 0;
}

