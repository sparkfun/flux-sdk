
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

// Global object -
_flxEventHub &flxEventHub = _flxEventHub::get();

void flxEventPost(flxEventID_t id)
{
    flxEventHub.postEvent(id);
}