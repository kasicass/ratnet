#include "ratnet.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void RNET_bzero(void *s, size_t n)
{
	memset(s, 0, n);
}

void RNET_errx(const char *msg)
{
	int my_errno = RNET_errno;

	fprintf(stderr, "%s : %d : %s\n", msg, my_errno, strerror(my_errno));
	exit(-1);
}

void RNET_dbgmsg(const char *msg)
{
	int my_errno = RNET_errno;

	fprintf(stdout, "%s : %d : %s\n", msg, my_errno, strerror(my_errno));
}

