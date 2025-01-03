//
// Ping.h
// 

#ifndef PING_H
#define PING_H

#include "Event.h"
#include "ViewObject.h"
#include "GameManager.h"

class Ping : public df::ViewObject {
private:

public:
	// Constructor.
	Ping();

	// Set set ping value
	void setPingValue(int value);

	// Handle PING events
	int eventHandler(const df::Event* p_e)  override;
};
#endif // PING_H