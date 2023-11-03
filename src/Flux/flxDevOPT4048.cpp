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
#include <cstdint>

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
    flxRegister(CIEx, "CIEx", "The X coordinate on the CIE 1931 Color Space Graph.");
    flxRegister(CIEy, "CIEy", "The Y coordinate on the CIE 1931 Color Space Graph.");
    flxRegister(Lux, "Lux", "The Lux value, or 'brightness'.");

    flxRegister(mode, "mode", "The Operation Mode: Power Down, Auto One Shot, One Shot, Continuous");
    flxRegister(time, "time", "Time spent converting analog values from internal sensors.");
    flxRegister(range, "range", "Range of light being sensed.");
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
    QwOpt4048::setI2CAddress(address());
    return QwOpt4048::beginI2C(wirePort);
}

// GETTER methods for output params
double flxDevOPT4048::get_CIEx()
{
    return QwOpt4048::getCIEx();
}

double flxDevOPT4048::get_CIEy()
{
    return QwOpt4048::getCIEy();
}

double flxDevOPT4048::get_CCT()
{
    return QwOpt4048::getCCT();
}

uint32_t flxDevOPT4048::get_lux()
{
    return QwOpt4048::getLux();
}

uint8_t flxDevOPT4048::get_range()
{
    opt4048_range_t _range = QwOpt4048::getRange();
    return static_cast<uint8_t>(range);
}

bool flxDevOPT4048::set_range(uint8_t range)
{
    uint8_t _range; 
    _range = static_cast<opt4048_range_t>(range);
    QwOpt4048::setRange(_range);
}

