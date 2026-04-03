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
 *  flxDevTMP102.h
 *
 *  Spark Device object for the TMP102 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevTMP102.h"

// Define our class static variables - allocs storage for them

uint8_t flxDevTMP102::defaultDeviceAddress[] = {0x48, 0x49, 0x4A, 0x4B, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevTMP102);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevTMP102::flxDevTMP102()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("TMP102 Precision Temperature Sensor");

    // Register parameters
    flxRegister(temperatureC, "Temperature (C)", "The temperature in degrees C", kParamValueTempC);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevTMP102::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t deviceID;
    if (!i2cDriver.readRegister16(address, TMP102_DEVICE_ID, &deviceID, false)) // Big Endian
        return false;

    return ((deviceID & 0xFFF) == DEVICE_ID_VALUE);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevTMP102::onInitialize(TwoWire &wirePort)
{

    bool result = TMP102::begin(address(), wirePort);
    if (result)
    {
        TMP102::setConversionAverageMode(0);
        TMP102::setConversionCycleBit(0);
        TMP102::setContinuousConversionMode();
    }
    return result;
}

// GETTER methods for output params
double flxDevTMP102::read_temperature_C()
{
    return (TMP102::readTempC());
}
