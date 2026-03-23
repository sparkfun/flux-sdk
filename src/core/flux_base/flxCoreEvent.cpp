
/*
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "flxCoreEvent.h"

#include "flxFlux.h"

// Declare the storage for the singleton
_flxEventHub &flxEventHub = _flxEventHub::get();

//---------------------------------------------------------------
// Implementation of the flxSendEvent() function
//
void flxSendEvent(flxEvent::flxEventID_t id)
{
    flxEventHub.sendEvent(id);
}
void flxSendEvent(flxEvent::flxEventIDNum_t num)
{
    flxEventHub.sendEvent(num);
}
void flxAddEventAlias(flxEvent::flxEventID_t id, flxEvent::flxEventID_t alias)
{
    flxEventHub.addEventAlias(id, alias);
}