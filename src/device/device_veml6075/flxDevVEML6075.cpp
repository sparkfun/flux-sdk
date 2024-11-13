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

/*
 *
 *  flxDevVEML6075.h
 *
 *  Spark Device object for the VEML6075 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevVEML6075.h"

// Define our class static variables - allocs storage for them

#define kVEML6075AddressDefault VEML6075_ADDRESS

uint8_t flxDevVEML6075::defaultDeviceAddress[] = {kVEML6075AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevVEML6075);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevVEML6075::flxDevVEML6075()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VEML6075 UV Sensor");

    // Register parameters
    flxRegister(uva, "UVA Level", "The UVA light level", kParamValueUVAIndex);
    flxRegister(uvb, "UVB Level", "The UVB light level", kParamValueUVBIndex);
    flxRegister(uvIndex, "UV Index", "The combined UV index", kParamValueUVIndex);

    // Register read-write properties
    flxRegister(integrationTime, "Integration Time (ms)", "The measurement integration time in milliseconds");
    flxRegister(highDynamic, "Dynamic Range", "The selected dynamic range: normal or high");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevVEML6075::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t deviceID;
    // VEML6075::REG_ID (0x0C) is private
    if (!i2cDriver.readRegister16(address, 0x0C, &deviceID, true)) // Little Endian
        return false;

    return (deviceID == 0x0026); // VEML6075_DEVICE_ID
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevVEML6075::onInitialize(TwoWire &wirePort)
{

    return VEML6075::begin(wirePort);
}

// GETTER methods for output params
float flxDevVEML6075::read_uva()
{
    return VEML6075::uva();
}

float flxDevVEML6075::read_uvb()
{
    return VEML6075::uvb();
}

float flxDevVEML6075::read_uv_index()
{
    return VEML6075::index();
}

// methods for read-write properties
uint8_t flxDevVEML6075::get_integration_time()
{
    if (isInitialized())
        _integrationTime = VEML6075::getIntegrationTime();
    return _integrationTime;
}

void flxDevVEML6075::set_integration_time(uint8_t intTime)
{
    _integrationTime = intTime;
    if (isInitialized())
        VEML6075::setIntegrationTime((VEML6075::veml6075_uv_it_t)intTime);
}

uint8_t flxDevVEML6075::get_high_dynamic()
{
    if (isInitialized())
        _highDynamic = VEML6075::getHighDynamic();
    return _highDynamic;
}

void flxDevVEML6075::set_high_dynamic(uint8_t high)
{
    _highDynamic = high;
    if (isInitialized())
        VEML6075::setHighDynamic((VEML6075::veml6075_hd_t)high);
}
