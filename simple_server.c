#include "ratnet.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(WIN32)
#if defined(_DEBUG)
#pragma comment(lib, "..\\debug\\ratnet_d.lib")
#else
#pragma comment(lib, "..\\release\\ratnet.lib")
#endif
#endif


void on_read(struct RNET_event *ev, void *arg)
{
	int n;
	struct RNET_buffer *buf;
	char data[80];
	RNET_socket fd = ev->fd;

	buf = RNET_buf_new();
	n = RNET_recvbuffer(fd, buf);
	// n = recv(fd, buf, sizeof(buf)-1, 0);
	if ( n == SOCKET_ERROR )
	{
		if (RNET_EAGAIN == RNET_errno )
		{
			puts("recv would block");
			return;
		}

		RNET_errx("recv() failed!");
	}

	if ( n == 0 )
	{
		printf("conn close: %d\n", fd);
		RNET_event_del(ev);
	}

	printf("int8  = %d\n", RNET_buf_pop_int8(buf));
	printf("int16 = %d\n", RNET_buf_pop_int16(buf));
	printf("int32 = %d\n", RNET_buf_pop_int32(buf));
	printf("string  = %s\n", RNET_buf_pop_string(buf));

	RNET_buf_pop_rawdata(buf, data, 2);
	data[2] = '\0';
	printf("rawdata = %s\n", data);
}

void on_accept(struct RNET_event *ev, void *arg)
{
	RNET_socket fd, client_fd;
	struct RNET_event *ev_read;

	fd = ev->fd;
	client_fd = RNET_accept(fd);
	if ( client_fd == INVALID_SOCKET )
		RNET_errx("accept() fail!");

	printf("accept ok\n");

	ev_read = RNET_event_new();
	RNET_event_set(ev_read, client_fd, EV_READ|EV_PERSIST, on_read, NULL);
	RNET_event_add(ev_read);
}


int main(void)
{
	RNET_socket listen_fd;
	struct RNET_event *ev_accept;


	RNET_init();

	listen_fd = RNET_create_tcp_socket();
	if ( INVALID_SOCKET == listen_fd )
		RNET_errx("RNET_create_tcp_socket() fail!");

	if ( RNET_bind_and_listen(listen_fd, RNET_INADDR_ANY, 5678) == SOCKET_ERROR )
		RNET_errx("RNET_bind_and_listen() fail!");


	ev_accept = RNET_event_new();
	RNET_event_set(ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	RNET_event_add(ev_accept);

	while (1)
	{
		RNET_event_loop(-1);
	}

	return 0;
}
