//
// ServerEntry.cpp
//

// Engine includes.
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "ServerEntry.h"
#include "util.h"
#include "Ping.h"

ServerEntry::ServerEntry() {
    setType("ServerEntry");
    setText("");
    setViewString("Server hostname: ");
    setBorder(true);
    setColor(df::YELLOW);
    setLocation(df::CENTER_CENTER);
    setLimit(MAX_WIDTH / 2); 
}

void ServerEntry::callback() {

    // Connect.
    std::string server_port = df::DRAGONFLY_PORT;
    LM.writeLog(1, "ServerEntry::callback(): Connecting to %s at server port %s.", getText().c_str(), server_port.c_str());
    
    if (NM.connect(getText(), server_port) < 0) {
        LM.writeLog("ServerEntry::callback(): ERROR cannot connect.");
        exit(-1);
    }

    LM.writeLog(1, "ServerEntry::callback(): Client connected.");

    // create Ping display.
    new Ping();

    WM.markForDelete(this);
}