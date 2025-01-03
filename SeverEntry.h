//
// ServerEntry.h
//

#ifndef SERVER_ENTRY_H
#define SERVER_ENTRY_H

// Engine includes.
#include "ViewObject.h"

// Define maximum width for input.
#define MAX_WIDTH 50

class ServerEntry : public df::ViewObject {
public:
    // Constructor.
    ServerEntry();

    // Callback when client enter server hostname.
    void callback();
};

#endif // SERVER_ENTRY_H

