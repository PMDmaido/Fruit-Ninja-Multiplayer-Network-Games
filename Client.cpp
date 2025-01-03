//
// Client.cpp
//

// System includes.
#include <string.h> // for memcpy()

// Engine includes.
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetworkCustom.h"
#include "EventStep.h"
#include "GameManager.h"
#include "LogManager.h"
#include "NetworkManager.h"
#include "WorldManager.h"
#include "EventNetworkCreate.h"

// Game includes.
#include "Client.h"
#include "Fruit.h"
#include "GameOver.h"
#include "Kudos.h"
#include "Points.h"
#include "ServerEntry.h"
#include "Sword.h"
#include "Timer.h"
#include "util.h"
#include "PingEvent.h"
#include "Ping.h"

Client::Client() {

    // Set as network client.
    setType(CLIENT_STRING);
    NM.setServer(false);

    // ServerEntry spawns others upon callback.
    if (new ServerEntry() == NULL) {
        LM.writeLog("Client::Client(): Error!  Cannot allocate ServerEntry.");
        exit(-1);
    }

    // Client handles mouse, sending it to Server.
    registerInterest(df::MSE_EVENT);

    // Keyboard for Q to quit.
    registerInterest(df::KEYBOARD_EVENT);

    // Send PING
    registerInterest(df::STEP_EVENT);

    // Latency compensate 
    registerInterest(df::NETWORK_CREATE_EVENT);

    LM.writeLog("Client::Client(): Client started.");

    // Initialize Variables
    ping_count = 0; // step count 15 ticks
    latency = 0;
    client_id = 0;

}

// Handle event.
int Client::eventHandler(const df::Event* p_e) {

    // Keyboard event.
    if (p_e->getType() == df::KEYBOARD_EVENT)
        return keyboard((df::EventKeyboard*)p_e);

    // Mouse event.
    if (p_e->getType() == df::MSE_EVENT)
        return mouse((df::EventMouse*)p_e);

    // Network event - custom "game over".
    if (p_e->getType() == df::NETWORK_CUSTOM_EVENT)
        return net((df::EventNetworkCustom*)p_e);

    if (p_e->getType() == df::STEP_EVENT)
        return step((df::EventStep*)p_e);

    if (p_e->getType() == df::NETWORK_CREATE_EVENT)
        return eventNetworkCreate((df::EventNetworkCreate*)p_e);

    // Call parent.
    return NetworkNode::eventHandler(p_e);
}

// Handle step event.
int Client::step(const df::EventStep* p_e) {

    ping_count++;
    // Every ping_delay steps, send a PING message.
    if (ping_count >= 15) {
        int ping_time = GM.getStepCount();  // Get the current step count.

        // Construct PING message: "Ping: <time>"
        char buff[50];
        sprintf_s(buff, "Ping %d", ping_time);

        // Send PING to the server
        if (sendMessage(df::MessageType::CUSTOM_MESSAGE, (int)strlen(buff), buff)) {
            LM.writeLog("Client::handleStep(): PING message sent.");
        }
        else {
            LM.writeLog(1, "Client::handleStep(): Error sending PING message.");
        }

        ping_count = 0;
    }
    return 1;
}

// Handle mouse event.
int Client::mouse(const df::EventMouse* p_e) {

    // If not connected, nothing to do.
    if (NM.isConnected() == false)
        return 0;

    // Check if mouse outside game window.
    sf::RenderWindow* p_win = DM.getWindow();
    sf::Vector2i pos = sf::Mouse::getPosition(*p_win);
    if (pos.x > df::Config::getInstance().getWindowHorizontalPixels() ||
        pos.x < 0 ||
        pos.y > df::Config::getInstance().getWindowVerticalPixels() ||
        pos.y < 0) {
        return 0; // Do nothing.
    }

    // If "move", send mouse position to server.
    if (p_e->getMouseAction() == df::MOVED) {

        df::EventMouseNetwork m_e;
        m_e.setSocketIndex(client_id);  
        m_e.setMousePosition(p_e->getMousePosition());  

        //// Send the mouse event over the network
        NM.onEvent(&m_e);

        // Sent mouse position to server
        int ret = sendMessage(df::MessageType::MOUSE_INPUT, df::MOVED,
            df::Mouse::UNDEFINED_MOUSE_BUTTON,
            p_e->getMousePosition());
        LM.writeLog(1, "Client::mouse(): Send mouse %s",
            p_e->getMousePosition().toString().c_str());
        return 1; // Handled.
    }
    // If get here, not handled.
    return 0;
}

// Handle keyboard event.
int Client::keyboard(const df::EventKeyboard* p_e) {

    // Only handle Q pressed.
    if (p_e->getKeyboardAction() == df::KEY_PRESSED &&
        p_e->getKey() == df::Keyboard::Q) {
        GM.setGameOver();
        return 1; // Handled.
    }
    // Not handled.
    return 0;
}

// Create Object of given type.
// Return pointer to Object (NULL if creation failed).
df::Object* Client::createObject(std::string obj_type) {

    LM.writeLog(1, "Client::createObject(): Creating %s",
        obj_type.c_str());

    df::Object* p_o = NULL;

    if (obj_type == SWORD_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Sword");
        p_o = (df::Object*) new Sword();
    }
    else if (obj_type == "pear" ||
        obj_type == "grapes" ||
        obj_type == "apple" ||
        obj_type == "banana" ||
        obj_type == "blueberries") {
        LM.writeLog(1, "Client::createObject(): Creating %s", obj_type.c_str());
        p_o = (df::Object*) new Fruit(obj_type);
    }
    else if (obj_type == POINTS_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Points");
        p_o = (df::Object*) new Points();
    }
    else if (obj_type == TIMER_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Timer");
        p_o = (df::Object*) new Timer();
    }
    else if (obj_type == KUDOS_STRING) {
        LM.writeLog(1, "Client::createObject(): Creating Kudos");
        p_o = (df::Object*) new Kudos();
    }
    else {
        LM.writeLog(1, "Client::createObject(): Unknown type: %s",
            obj_type.c_str());
    }

    return p_o;
}

// Handle custom network event.
int Client::net(const df::EventNetworkCustom* p_en) {

    // Extract message from event
    const char* msg = static_cast<const char*>(p_en->getMessage());

    // Check message type
    // Handle PING message
    if (strncmp(msg, "Ping", 4) == 0) {
        
        char sent_t[50];
        // Skip "Ping_"(5 bytes) and extract int time 
        memcpy(&sent_t, msg + 5, sizeof(int));

        // Calculate latency in game ticks
        int latency_ticks = GM.getStepCount() - atoi(sent_t);

        // Update latency
        latency = latency_ticks;

        // ticks to milliseconds
        int latency_ms = latency_ticks * GM.getFrameTime();

        // Trigger PING_EVENT or update Ping view object
        PingEvent ping_event(latency_ms);

        // Send the PingEvent to the WorldManager
        WM.onEvent(&ping_event);

        LM.writeLog("Client::net(): Receive PING echo. Latency: %d ms", latency_ms);
        return 1; // Handled
    }
    // Handle GAME OVER message
    else if (strcmp(msg, "game over") == 0) { // Game Over
        new GameOver();
        LM.writeLog(1, "Client::net(): Receive 'game over' message.");
        return 1;
    }
    // Handle INDEX message
    else if (strncmp(msg, "index", 5) == 0) {
        // Store client index
        char index[50];
        memcpy(&index, msg + 5, sizeof(int));

        client_id = atoi(index); 
        LM.writeLog("This client socket index  is %d", client_id);

        return 1;
    }
    // Other message
    LM.writeLog(1, "Client::net(): ERROR Unexpected message '%s'", msg);

    return 1; // Handled
}

int Client::handleClose(const df::EventNetwork* p_en) {
    LM.writeLog(1, "Client::handleClose():");
    GM.setGameOver();
    return 1;
}

int Client::eventNetworkCreate(const df::EventNetworkCreate* p_en) {

  
    df::Object* p_o = p_en->getObject();

    if (dynamic_cast<Fruit*>(p_o)) {
        for (int i = 0; i < latency; i++) {
            df::Vector new_pos = p_o->predictPosition();
            LM.writeLog(1, "Client::eventNetworkCreate(): Latency: %d tick, Predict next position: (%f, %f)",
                i, new_pos.getX(), new_pos.getY());
            WM.moveObject(p_o, new_pos); 
        }
    }
    LM.writeLog("Client::eventNetworkCreate(): Handled object ID: %d.", p_o->getId());


    return 1;
}