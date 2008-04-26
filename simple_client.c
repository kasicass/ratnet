#include "ratnet.h"
#include <stdio.h>

#if defined(WIN32)
#if defined(_DEBUG)
#pragma comment(lib, "..\\debug\\ratnet_d.lib")
#else
#pragma comment(lib, "..\\release\\ratnet.lib")
#endif
#endif


void on_write(struct RNET_event *ev, void *arg)
{
	int n;
	struct RNET_buffer *buf;
	RNET_socket fd = ev->fd;

	printf("begin write\n");
	buf = RNET_buf_new();
	RNET_buf_push_int8(buf, 64);
	RNET_buf_push_int16(buf, 64);
	RNET_buf_push_int32(buf, 64);

	RNET_buf_push_string(buf, "abc");		// c-style
	RNET_buf_push_rawdata(buf, "abcd", 2);		// c-style

	puts("send ...");
	n = RNET_sendbuffer(fd, buf);
	RNET_buf_free(buf);
	printf("n = %d\n", n);
	if ( n == SOCKET_ERROR )
		RNET_errx("RNET_sendbuffer() error!");

#if defined(RATNET_WIN32)
	Sleep(1000);
#else
	sleep(1);
#endif
}

int main(void)
{
	RNET_socket sock_fd;
	struct RNET_event *ev_write;

	RNET_init();

	sock_fd = RNET_create_tcp_socket();
	if ( INVALID_SOCKET == sock_fd )
		RNET_errx("RNET_create_tcp_socket() fail!");

	if ( RNET_connect(sock_fd, "192.168.1.177", 5678) == SOCKET_ERROR )
		RNET_errx("RNET_connect() failed!");

	puts("connect ok");


	ev_write = RNET_event_new();
	RNET_event_set(ev_write, sock_fd, EV_WRITE|EV_PERSIST, on_write, NULL);
	RNET_event_add(ev_write);

	while (1)
	{
		RNET_event_loop(NULL);
		// puts("my loop");
	}

	return 0;
}

