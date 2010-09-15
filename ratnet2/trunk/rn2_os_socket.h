#ifndef KCODE_RATNET2_OS_SOCKET_H
#define KCODE_RATNET2_OS_SOCKET_H

#include <sys/time.h>
#include <sys/types.h>          // size_t
#include <sys/socket.h>
#include <sys/event.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

namespace RN2 { namespace OS {

int setnonblock(int fd);
int new_listen_socket(const char *ip, int port);

}}

#endif
