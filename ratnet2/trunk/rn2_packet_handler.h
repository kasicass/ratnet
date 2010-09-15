#ifndef KCODE_RATNET2_PACKET_HANDLER_H
#define	KCODE_RATNET2_PACKET_HANDLER_H

namespace RN2 {

class Packet;

class PacketHandler {
public:
	PacketHandler() {};
	virtual ~PacketHandler() {};

	virtual void OnRecv(RN2::Packet *packet) = 0;
};

}

#endif
