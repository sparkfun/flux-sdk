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
 *  flxDevMicroOLED.h
 *
 *  Spark Device object for the Micro OLED device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevMicroOLED.h"

// Define our class static variables - allocs storage for them

uint8_t flxDevMicroOLED::defaultDeviceAddress[] = {kOLEDMicroDefaultAddress, kOLEDMicroAltAddress, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevMicroOLED);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevMicroOLED::flxDevMicroOLED()
{
    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName(), "Qwiic Micro OLED Display");

}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevMicroOLED::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    return i2cDriver.ping(address);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMicroOLED::onInitialize(TwoWire &wirePort)
{
    return QwiicMicroOLED::begin(wirePort, address());
}