/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#pragma once

#include "flxCoreEvent.h"

// define the on connection change event type
flxDefineEventID(kOnConnectionChange);

// Network interface
class flxNetwork
{
  public:
    virtual bool isConnected() = 0;
    virtual IPAddress localIP(void) = 0;
};