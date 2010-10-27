#ifndef KCODE_RATNET2_SIGNAL_HANDLER_H
#define KCODE_RATNET2_SIGNAL_HANDLER_H

namespace RN2 {

class SignalHandler {
public:
	SignalHandler() {}
	virtual ~SignalHandler() {}

	virtual void OnSignal(int sig) = 0;
};

void SetSignalHandler(int sig, SignalHandler *sh);

}

#endif
