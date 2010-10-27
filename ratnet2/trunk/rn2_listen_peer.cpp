#include "rn2_listen_peer.h"
#include "rn2_os_socket.h"
#include <cassert>
#include <stdlib.h>
#include <event.h>

namespace RN2 {

ListenPeer::ListenPeer(Acceptor *acceptor_)
{
    assert(acceptor_ != NULL);
    this->acceptor_ = acceptor_;
}

ListenPeer::~ListenPeer()
{
}

void ListenPeer::AddEndpoint(const char *desc1)
{
    std::string desc = desc1;
    size_t pos;
    pos = desc.find(':');
    assert(pos != std::string::npos);

    struct EndpointDesc ed;
    ed.ip   = desc.substr(0, pos);
    ed.port = atoi(desc.substr(pos+1).c_str());
    
    endpoints_.push_back(ed);
}

static void ListenPeerCallback(int listenfd, short events, void *arg)
{
    int fd = accept(listenfd, NULL, NULL);
    printf("conn from %d ...\n", fd);
    close(fd);
}

void ListenPeer::Bind() {
    assert(endpoints_.size() != 0);

    for ( size_t i = 0; i < endpoints_.size(); i++ )
    {
        struct EndpointDesc &ed = endpoints_[i];
        int listenfd = OS::new_listen_socket(ed.ip.c_str(), ed.port);

        ed.ev = (struct event *) malloc(sizeof(struct event));
        event_set(ed.ev, listenfd, EV_READ|EV_PERSIST, ListenPeerCallback, this);
        event_add(ed.ev, NULL);
    }
}

}

