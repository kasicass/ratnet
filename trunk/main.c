#include "ratnet.h"
#include <stdio.h>


void on_read(RNET_socket fd, int ev, void *arg)
{
	int n;
	char buf[512];

	n = recv(fd, buf, sizeof(buf)-1, 0);
	if ( n == SOCKET_ERROR )
	{
		if (WSAEWOULDBLOCK == RNET_errno )
		{
			puts("recv would block");
			return;
		}

		RNET_errx("recv() failed!");
	}

	if ( n == 0 )
	{
		RNET_errx("client connect close");
	}

	buf[n] = '\0';
	printf("recv = %s\n", buf);
}

void on_accept(RNET_socket fd, int ev, void *arg)
{
	RNET_socket client_fd;
	struct RNET_event ev_read;

	client_fd = RNET_accept(fd);
	if ( client_fd == INVALID_SOCKET )
		RNET_errx("accept() fail!");

	printf("accept ok\n");
	
	RNET_event_set(&ev_read, client_fd, EV_READ|EV_PERSIST, on_read, NULL);
	RNET_event_add(&ev_read);
}


int main(void)
{
	RNET_socket listen_fd;
	struct RNET_event   ev_accept;
	struct timeval		timeout = { 1, 0 };


	RNET_init();

	listen_fd = RNET_create_tcp_socket();
	if ( INVALID_SOCKET == listen_fd )
		RNET_errx("RNET_create_tcp_socket() fail!");

	if ( RNET_bind_and_listen(listen_fd, "127.0.0.1", 8888) == SOCKET_ERROR )
		RNET_errx("RNET_bind_and_listen() fail!");


	RNET_event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	RNET_event_add(&ev_accept);

	while (1)
	{
		RNET_event_loop(&timeout);
	}

	return 0;
}
