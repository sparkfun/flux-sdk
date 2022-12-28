

#pragma once

#include "flxCoreEvent.h"
// Network interface
class spNetwork
{
public:
    virtual bool isConnected() = 0;

    // Event - triggered on connection changes
    flxSignalBool on_connectionChange;

};