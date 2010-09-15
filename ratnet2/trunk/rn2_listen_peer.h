#ifndef KCODE_RATNET2_LISTEN_PEER_H
#define KCODE_RATNET2_LISTEN_PEER_H

#include <string>
#include <vector>

struct event;

namespace RN2 {

class Acceptor;

class ListenPeer {
public:
	enum {MAX_ENDPOINT = 5};

public:
	ListenPeer(Acceptor *acceptor);
	~ListenPeer();

	// desc = "ip:port"
	// true - ok, false - fail
	void AddEndpoint(const char *desc);
	void Bind();

private:
	struct EndpointDesc { std::string ip; int port; struct event *ev; };
	std::vector<struct EndpointDesc> endpoints_;

	Acceptor *acceptor_;
};

}

#endif
