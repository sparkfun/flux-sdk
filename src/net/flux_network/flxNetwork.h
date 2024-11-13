/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
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