#include "ratnet.h"
#include <string.h>
#include <stdio.h>


void RNET_bzero(void *s, size_t n)
{
	memset(s, 0, n);
}

void RNET_errx(const char *msg)
{
	puts(msg);
	exit(-1);
}

void RNET_dbgmsg(const char *msg)
{
	puts(msg);
}