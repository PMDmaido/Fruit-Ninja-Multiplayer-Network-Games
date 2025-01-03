//
// Client.h
//

#ifndef CLIENT_H
#define CLIENT_H

// Engine includes.
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetwork.h"
#include "EventNetworkCustom.h"
#include "NetworkNode.h"
#include "EventNetworkCreate.h"
#include "EventStep.h"

const std::string CLIENT_STRING = "Client";

class Client : public df::NetworkNode {

 public:
  Client();

  // Handle event.
  int eventHandler(const df::Event *p_e) override;

  // Handle close event.
  int handleClose(const df::EventNetwork *p_en) override;

  // Create Object of given type.
  // Return pointer to Object.
  df::Object *createObject(std::string obj_type) override; 

  int eventNetworkCreate(const df::EventNetworkCreate* p_en);

 private:
	 int ping_count;
	 int latency;
	 int client_id;

  // Handle mouse event.
  int mouse(const df::EventMouse *p_e);
  
  // Handle keyboard event.
  int keyboard(const df::EventKeyboard *p_e);

  // Handle custom network event.
  int net(const df::EventNetworkCustom *p_en);

  // Handle step event: Ping
  int step(const  df::EventStep *p_e);

};

#endif
