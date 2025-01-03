//
// PingEvent.cpp
//
#include "PingEvent.h"

PingEvent::PingEvent(int latency) {
    setType(PING_EVENT);
    latency_ms = latency;
}

int PingEvent::getLatency() const {
    return latency_ms;
}