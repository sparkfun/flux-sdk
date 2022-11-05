

#pragma once

#include "spCoreEvent.h"
// Network interface
class spNetwork
{
public:
    virtual bool isConnected() = 0;

    // Event - triggered on connection changes
    spSignalBool on_connectionChange;

};