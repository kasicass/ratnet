#include "rn2_signal_handler.h"
#include <event.h>
#include <stdlib.h>

namespace RN2 {

static void EventSignalHandler(int sig, short events, void *arg)
{
    SignalHandler *sh = reinterpret_cast<SignalHandler*>(arg);
    sh->OnSignal(sig);
}

void SetSignalHandler(int sig, SignalHandler *sh) {
    struct event *ev;
    ev = (struct event *) malloc(sizeof(*ev));   // TODO: ev not free
    signal_set(ev, sig, EventSignalHandler, reinterpret_cast<void*>(sh));
    signal_add(ev, NULL);
}

}

