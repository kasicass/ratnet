#include "ratnet.h"
#include <string.h>
#include <stdio.h>


void RNET_bzero(void *s, size_t n)
{
	memset(s, 0, n);
}

void RNET_errx(const char *msg)
{
	printf("%s : %d\n", msg, RNET_errno);
	exit(-1);
}

void RNET_dbgmsg(const char *msg)
{
	puts(msg);
}