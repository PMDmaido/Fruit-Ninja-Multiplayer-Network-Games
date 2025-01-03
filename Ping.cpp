// Engine includes
#include "Event.h"
#include "EventStep.h"
#include "NetworkManager.h"

#include "ViewObject.h"
#include "GameManager.h"
#include "LogManager.h"

// Game includes
#include "Ping.h"
#include "PingEvent.h"
#include "util.h"

Ping::Ping() {
          
	setViewString("Ping: ");
	setLocation(df::TOP_RIGHT); 
	setColor(df::WHITE);          
	setValue(0);                  

	// Register interest in custom PING events.
	registerInterest(PING_EVENT);
}

// Set ping value (latency) and update color based on latency.
void Ping::setPingValue(int value) {

	setValue(value);

	// Update the color based on latency.
	if (value < 100) {
		setColor(df::GREEN);  // Green <100ms
	}
	else if (value >= 100 && value < 300) {
		setColor(df::YELLOW);  // Yellow 100-300ms
	}
	else {
		setColor(df::RED);     // Red >300ms
	}
	LM.writeLog("Ping::setPingValue(): Updated PING to %d ms.", value);
}

int Ping::eventHandler(const df::Event* p_e) {
	if (p_e->getType() == PING_EVENT) {
		const PingEvent* ping_e = dynamic_cast<const PingEvent*>(p_e);
		if (ping_e) {
			// Set the new ping value.
			setPingValue(ping_e->getLatency());
			return 1;
		}
		// if PING event is not handled, call parent handler
		return df::ViewObject::eventHandler(p_e);
	}
}



