#include "rn2_os_socket.h"

namespace RN2 { namespace OS {

int setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        goto label_nonblock_error;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
        goto label_nonblock_error;

    return 0;

label_nonblock_error:
    return -1;
}

int new_listen_socket(const char *ip, int port)
{
    int fd;
    struct sockaddr_in in_addr;
    int reuseaddr_on = 1;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( fd < 0 ) return -1;

    // addr reuse
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseaddr_on,
            sizeof(reuseaddr_on)) == -1)
    {
        return -1;
    }

    // bind & listen
    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family      = AF_INET;
    in_addr.sin_addr.s_addr = strcmp(ip, "any") == 0 ? htonl(INADDR_ANY) : inet_addr(ip);
    in_addr.sin_port        = htons(port);

    if ( bind(fd, (struct sockaddr *) &in_addr, sizeof(in_addr)) == -1 )
        return -1;

    if ( listen(fd, 64) == -1 )
        return -1;

    // set non-block
    if ( setnonblock(fd) == -1 )
        return -1;

    return fd;
}

}}
