
/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#include "flxCoreEvent.h"

#include "flxFlux.h"

// Declare the storage for the singleton
_flxEventHub &flxEventHub = _flxEventHub::get();

//---------------------------------------------------------------
// Implementation of the flxSendEvent() function
//
void flxSendEvent(flxEventID_t id)
{
    flxEventHub.sendEvent(id);
}