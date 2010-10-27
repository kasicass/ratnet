// ListenPeer ==> Endpoint1
//            \=> Endpoint2
//                ...
//
// ClientPeer <==> ClientPeer

#include "ratnet2.h"

class MyAcceptor : public RN2::Acceptor {
public:
	virtual void OnConnectionMake(RN2::Socket *s) {
	}

	virtual void OnConnectionLost(RN2::PacketHandler *ph) {
	}
};

class MySignalHandler : public RN2::SignalHandler {
public:
	virtual void OnSignal(int sig) {
		printf("sigusr1\n");
		RN2::QuitLoop();
	}
};

int main()
{
    RN2::Init();

	RN2::ListenPeer *lp = new RN2::ListenPeer(new MyAcceptor);
	lp->AddEndpoint("127.0.0.1:2222");
	lp->AddEndpoint("127.0.0.1:3333");
	lp->Bind();

	RN2::SetSignalHandler(SIGUSR1, new MySignalHandler);
	RN2::EventLoop();
    
    RN2::Shutdown();
	return 0;
}

