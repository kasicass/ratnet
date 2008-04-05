#include "ratnet.h"
#include <stdio.h>


/*
struct client_data {
	struct RNET_event ev_read;
	RNET_socket fd;
	// ...
};


void my_errx(const char *msg)
{
	puts(msg);
	exit(-1);
}


void on_read(RNET_socket fd, short ev, void *arg)
{
	struct RNET_buffer buf;

	RNET_read(fd, &buf);
	buf.nbytes_read
}

void
on_accept(RNET_socket fd, short ev, void *arg)
{
	RNET_socket client_fd;
	struct client_data *cd = malloc(..);

	client_fd = RNET_accept();

	cd->fd = client_fd;
	RNET_event_set(&cd->ev_read, client_fd, EV_READ|EV_PERSIST, on_read, fd);
	RNET_event_add(&cd->ev_read, NULL);
}
*/


#define	BUF_SIZE		1024

int main(void)
{
	RNET_socket listen_fd;
	RNET_socket client_fd;
	char buf[BUF_SIZE];
	int n;
//	struct RNET_event   ev_accept;

	
	RNET_init();

	listen_fd = RNET_create_tcp_socket();
	if ( INVALID_SOCKET == listen_fd )
		RNET_errx("RNET_create_tcp_socket() fail!");

	if ( RNET_bind_and_listen(listen_fd, "127.0.0.1", 8888) == SOCKET_ERROR )
		RNET_errx("RNET_bind_and_listen() fail!");


	puts("begin accept ...");
	client_fd = accept(listen_fd, NULL, NULL);

	puts("begin recv ...");
	n = recv(client_fd, buf, BUF_SIZE, 0);
	buf[n] = '\0';
	printf("recv = %s\n", buf);

	closesocket(client_fd);
	closesocket(listen_fd);
	
	/*

	RNET_event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	RNET_event_add(&ev_accept, NULL);

	while (1)
	{
		RNET_event_loop( timeout(1, 0) );

	}
	*/

	return 0;
}
