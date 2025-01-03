//
// Server.cpp
//

// System includes.
#include <string.h> // for memcpy()

// Engine includes.
#include "EventNetwork.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"

// Game includes.
#include "Fruit.h"
#include "GameOver.h"
#include "Grocer.h"
#include "Kudos.h"
#include "Server.h"
#include "Sword.h"
#include "Timer.h"
#include "util.h"
#include <EventNetworkCustom.h>

Server::Server() {

    // Initialize member attributes.
    for (int i = 0; i < MAX_PLAYERS; i++) {
        p_sword[i] = NULL;
        p_points[i] = NULL;
    }

    // Set as network server.
    setType(SERVER_STRING);
    if (NM.setServer(true) != 0) {
        LM.writeLog("Server::Server(): Failed to set server mode.");
        exit(-1);
    }
    NM.setMaxConnections(MAX_PLAYERS);

    // Register for step events for sync.
    registerInterest(df::STEP_EVENT);

    LM.writeLog("Server::Server(): Server started. Need %d players", MAX_PLAYERS);
}

// Handle event.
// Return 0 if ignored, else 1.
int Server::eventHandler(const df::Event* p_e) {

    // Step event.
    if (p_e->getType() == df::STEP_EVENT && NM.isConnected())
        return handleStep((const df::EventStep*)p_e);

    // Custom network event PING
    if (p_e->getType() == df::NETWORK_CUSTOM_EVENT)
        return handleEventNetworkCustom((df::EventNetworkCustom*)p_e);

    // Call parent event handler.
    return NetworkNode::eventHandler(p_e);

} // End of eventHandler().

int Server::handleAccept(const df::EventNetwork* p_en) {
    LM.writeLog("Server::handleAccept(): Server connected to socket: %d", p_en->getSocketIndex());

    int sock_index = p_en->getSocketIndex();

   // Set delay this client.
    if (NM.setDelay(DELAY, sock_index) != 0) {
        LM.writeLog("Server::handleAccept(): ERROR setting delay for socket %d", sock_index);
    }
    else {
        LM.writeLog("Server::handleAccept(): Set delay %d ms for socket %d", DELAY, sock_index);
    }

    //Send socket index to player
    char buff[50];
    sprintf_s(buff, "index %d", sock_index);
    sendMessage(df::MessageType::CUSTOM_MESSAGE, (int)strlen(buff), buff, sock_index);

    // If enough players connected or Grocer started, nothing else to do.
    if (NM.getNumConnections() < MAX_PLAYERS || WM.objectsOfType("Grocer").getCount() == 1)
        return 1;

    // Otherwise, start game.

    // Create Swords.
    for (int i = 0; i < NM.getNumConnections(); i++) {
        Sword* p_s = new Sword();
        if (!p_s) {
            LM.writeLog("Server::handleAccept(): ERROR  Cannot allocate Sword.");
            exit(-1);
        }
        p_s->setColor(sockToColor(i));
        p_s->setId(SWORD_ID + i);
        p_s->setSocketIndex(i);
        p_sword[i] = p_s;
        LM.writeLog(1, "Server::handleAccept(): Sword %d created.", i);

        sendMessage(df::MessageType::SYNC_OBJECT, p_s);
    }

    // Create Points.
    for (int i = 0; i < NM.getNumConnections(); i++) {
        Points* p_p = new Points();
        if (!p_p) {
            LM.writeLog("Server::handleAccept(): ERROR  Cannot allocate Points.");
            exit(-1);
        }
        p_p->setLocation(sockToLocation(i));
        std::string s = "Player ";
        s += df::toString(i);
        s += ":";
        p_p->setViewString(s);
        p_points[i] = p_p;
        LM.writeLog(1, "Server::handleAccept(): Points %d created.", i);
    }

    // Grocer to start spawning Fruit.
    new Grocer();

    // Timer for time progress.
    new Timer();

    LM.writeLog(1, "Server::handleAccept(): Game has begun.");

    return 1;
}

// If any Objects need to be synchronized, send to Clients.
int Server::handleStep(const df::EventStep* p_es) {

    // Iterate through all Objects.
    df::ObjectList ol = WM.getAllObjects();
    for (int i = 0; i < ol.getCount(); i++) {
        Object* p_o = ol[i];

        // (See project description writeup for what & when to sync.)
        int sock_index = -2; // -2 means don't sync
        if (p_o->getType() == SWORD_STRING) {
            if (p_o->isModified(df::ObjectAttribute::POSITION)) {
                Sword* p_s = dynamic_cast <Sword*> (p_o);
                if (p_s->getSocketIndex() == 1) {
                    sock_index = 0;
                }
                else {
                    sock_index = 1;
                }
            }
        }
        if ((dynamic_cast <Fruit*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID))
                sock_index = -1; // sync with all
        if ((dynamic_cast <Timer*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID))
                sock_index = -1; // sync with all
        if ((dynamic_cast <Points*> (p_o))) {
            unsigned int mask = (unsigned int)df::ObjectAttribute::ID |
                (unsigned int)df::ViewObjectAttribute::VALUE;
            if (p_o->isModified((df::ObjectAttribute)mask))
                sock_index = -1; // sync with all
        }
        if ((dynamic_cast <Kudos*> (p_o)))
            if (p_o->isModified(df::ObjectAttribute::ID)) {
                Kudos* p_k = dynamic_cast <Kudos*> (p_o);
                sock_index = p_k->getSocketIndex(); // sync with only that client
            }

        // If needed, send to client(s).
        if (sock_index != -2) {
            LM.writeLog(1, "Server::handleStep(): SYNC %s (id %d), sock_index %d",
                p_o->getType().c_str(),
                p_o->getId(),
                sock_index);
            if (sendMessage(df::MessageType::SYNC_OBJECT, p_o, 0, sock_index) == -1) {
                LM.writeLog("Server::handleStep(): ERROR after sendMessage().");
                exit(-1);
            }
        }

    } // End of iterate through all Objects.

    return 1;
}

// Handle custom PING messages from clients.
int Server::handleEventNetworkCustom(const df::EventNetworkCustom* p_en) {

    // getMessage of client
    const void* message = p_en->getMessage();

    // Cast the const void* to const char*
    const char* charMessage = static_cast<const char*>(message);

    char buff[50];
    strcpy_s(buff, charMessage);

    // Send PING to all connected clients.
    if (sendMessage(df::MessageType::CUSTOM_MESSAGE, (int)strlen(buff), buff, p_en->getSocketIndex())) {
        LM.writeLog("Server::handleData(): PING sent to all clients.");
        return 0;
    }
    else {
        LM.writeLog("Server::handleData(): ERROR sending PING.");
        return -1;
    }
    return 0; // Not handled.
}

// Handle close event.
int Server::handleClose(const df::EventNetwork* p_en) {

    int sock_index = p_en->getSocketIndex();
    LM.writeLog(1, "Server::handleClose(): socket %d", sock_index);

    // If any client leaves, shut down.
    GM.setGameOver();

    return 1;
}