/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevBMV080.h
 *
 *  Spark Device object for the VEML7700 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBMV080.h"

// Define our class static variables - allocs storage for them

#define kBMV080AddressDefault SF_BMV080_DEFAULT_ADDRESS

uint8_t flxDevBMV080::defaultDeviceAddress[] = {kBMV080AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBMV080);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBMV080::flxDevBMV080()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("BMV080 Particulate Matter Sensor");

    // Register parameters

    // Register read-write properties
    flxRegister(PM10, "PM10", "The PM10 concentration in micrograms per cubic meter (µg/m³)");
    flxRegister(PM25, "PM25", "The measurement sensitivity");
    flxRegister(PM1, "PM1", "The measurement persistence");
    flxRegister(obstructed, "Obstructed", "Is the sensor obstructed?");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBMV080::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // The BMV080 just has support for PING detection at the default address
    return i2cDriver.ping(address);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBMV080::onInitialize(TwoWire &wirePort)
{

    if (SparkFunBMV080::begin(address(), wirePort) == false)
    {
        flxLog_D(F("BMV080: Failed to initialize device at address 0x%02X"), address());
        return false;
    }

    if (SparkFunBMV080::init() == false)
    {
        flxLog_D(F("BMV080: Failed to initialize device"));
        return false;
    }

    if (SparkFunBMV080::setMode(SF_BMV080_MODE_CONTINUOUS) == false)
    {
        flxLog_D(F("BMV080: Failed to set continuous mode"));
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------------------------------------
// execute()
//
bool flxDevBMV080::execute(void)
{
    return SparkFunBMV080::readSensor();
}
