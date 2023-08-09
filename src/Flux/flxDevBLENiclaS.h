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
 
/*
 *
 *  flxDevBME280.h
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDeviceBLE.h"


#define kBLENiclaSenseUUID "19b10000-0000-537e-4f6c-d104768a1214"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBLENiclaS : public flxDeviceBLEType<flxDevBLENiclaS>
{

public:
    flxDevBLENiclaS();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static const char *getDeviceName()
    {
        return "Arduino Nicla Sense";
    };

    static const char *getServiceUUID(void)
    {
        return kBLENiclaSenseUUID;
    }
    

    // Method called to initialize the class
    bool onInitialize(BLERemoteService *pService);


};
