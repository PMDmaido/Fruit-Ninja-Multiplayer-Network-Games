//
// Server.h
//

#ifndef SERVER_H
#define SERVER_H

// Engine includes.
#include "NetworkNode.h"
#include "EventNetworkCustom.h"

// Game includes.
#include "Points.h"
#include "Sword.h"
#include "util.h"

const std::string SERVER_STRING = "Server";

#define SERVER ((Server *) WM.objectsOfType(SERVER_STRING)[0])

class Server : public df::NetworkNode {

 private:
  Sword *p_sword[MAX_PLAYERS];    // Sword for each client.
  Points *p_points[MAX_PLAYERS];  // Points for each client.

 public:
  Server();

  // Handle event.
  int eventHandler(const df::Event *p_e) override;

  // Handle accept event.
  int handleAccept(const df::EventNetwork *p_en) override;

  // Handle close event.
  int handleClose(const df::EventNetwork *p_en) override;

  // Handle Custom EventNetwork data (Ping) from clients. 
  int handleEventNetworkCustom(const df::EventNetworkCustom* p_en);

private:  
  // Handle step event.
  int handleStep(const df::EventStep *p_es);

};

#endif
