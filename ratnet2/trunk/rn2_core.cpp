#include "rn2_core.h"
#include <event.h>

namespace RN2 {

void Init() {
    event_init();
}

void Shutdown() {
}

void EventLoop() {
    event_dispatch();
}

void QuitLoop() {
    event_loopexit(NULL);
}

}

