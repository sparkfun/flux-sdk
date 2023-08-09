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
 *  flxDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBLENiclaS.h"

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDeviceBLE(flxDevBLENiclaS);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBLENiclaS::flxDevBLENiclaS()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Example of a BLE Device - using a Arduino Nicla Sense");
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBLENiclaS::onInitialize(BLERemoteService *pService)
{
    return true;
}