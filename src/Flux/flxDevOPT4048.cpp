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
 *
 *  flxDevOPT4048.cpp
 *
 *  Device object for the OPT4048 Color Sensor Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevOPT4048.h"

#define OPT4048_CHIP_ID_REG 0x11 // Register containing the chip ID
#define OPT4048_UNIQUE_ID 0x2048 // Chip ID

#define kOPTAddressDefault 0x44
#define kOPTAddressAlt1 0x45
#define kOPTAddressAlt2 0x46

// Define our class static variables - allocs storage for them

uint8_t flxDevOPT4048::defaultDeviceAddress[] = {kOPTAddressDefault, kOPTAddressAlt1, kOPTAddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevOPT4048);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevOPT4048::flxDevOPT4048()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Texas Instrument OPT4048 Tristimulus Color Sensor");

    // Register parameters
    flxRegister(CIEx, "Humidity", "The sensed humidity value");
    flxRegister(CIEy, "TemperatureF", "The sensed Temperature in degrees Fahrenheit");
    flxRegister(Lux, "TemperatureC", "The sensed Temperature in degrees Celsius");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevOPT4048::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t chipID = i2cDriver.readRegisterRegion(address, OPT4048_CHIP_ID_REG); // Should return 0x2048

    // flxLog_I("OPT4048 isConnected chip ID 0x%02x", chipID);

    return (chipID == OPT4048_UNIQUE_ID);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevOPT4048::onInitialize(TwoWire &wirePort)
{

    // set the device address
    OPT4048::setI2CAddress(address());
    return OPT4048::beginI2C(wirePort);
}

// GETTER methods for output params
float flxDevOPT4048::read_Humidity()
{
    return OPT4048::readFloatHumidity();
}

float flxDevOPT4048::read_TemperatureF()
{
    return OPT4048::readTempF();
}
float flxDevOPT4048::read_TemperatureC()
{
    return OPT4048::readTempC();
}

float flxDevOPT4048::read_Pressure()
{
    return OPT4048::readFloatPressure();
}

float flxDevOPT4048::read_AltitudeM()
{
    return OPT4048::readFloatAltitudeMeters();
}
loat flxDevOPT4048::read_AltitudeF()
{
    return OPT4048::readFloatAltitudeFeet();
}
