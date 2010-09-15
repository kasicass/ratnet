#ifndef KCODE_RATNET2_ACCEPTOR_H
#define	KCODE_RATNET2_ACCEPTOR_H

namespace RN2 {

class Socket;
class PacketHandler;

class Acceptor {
public:
	Acceptor() {}
	virtual ~Acceptor() {}

	virtual void OnConnectionMake(RN2::Socket *s) = 0;
	virtual void OnConnectionLost(RN2::PacketHandler *ph) = 0;
};

}

#endif
