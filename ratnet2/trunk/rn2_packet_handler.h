#ifndef KCODE_RATNET2_PACKET_HANDLER_H
#define	KCODE_RATNET2_PACKET_HANDLER_H

namespace RN2 {

class Packet;
class Socket;

class PacketHandler {
public:
	PacketHandler(Socket *s);
	virtual ~PacketHandler();

    Socket *GetSocket();

	virtual void OnRecv(RN2::Packet *packet) = 0;

private:
    Socket *socket_;
};

}

#endif
