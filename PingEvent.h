//
// PingEvent.h
// 

#ifndef PINGEVENT_H
#define PINGEVENT_H

#include "GameManager.h"
#include "Event.h"

const std::string PING_EVENT = "ping";

class PingEvent : public df::Event {
private:
	int latency_ms;

public:
	// Constructor.
	PingEvent(int latency);

	// Get the ping latency.
	int getLatency() const;
};

#endif // PINGEVENT_H