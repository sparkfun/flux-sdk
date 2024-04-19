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

#define kOPTAddressDefault OPT4048_ADDR_DEF
#define kOPTAddressAlt1 OPT4048_ADDR_SCL
#define kOPTAddressAlt2 OPT4048_ADDR_SDA

// Define our class static variables - allocs storage for them

uint8_t flxDevOPT4048::defaultDeviceAddress[] = {kOPTAddressDefault, kOPTAddressAlt1, kOPTAddressAlt2,
                                                 kSparkDeviceAddressNull};

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
    setName(getDeviceName(), "OPT4048 Tristimulus Color Sensor");

    // Register parameters
    flxRegister(CIEx, "CIEx", "The X coordinate on the CIE 1931 Color Space Graph");
    flxRegister(CIEy, "CIEy", "The Y coordinate on the CIE 1931 Color Space Graph");
    flxRegister(CCT, "CCT", "The Correlated Color Temperature (CCT) of the sensor (K)");
    flxRegister(Lux, "Lux", "The Lux value, or 'brightness'.");

    flxRegister(mode, "mode", "The Operation Mode: Power Down, Auto One Shot, One Shot, Continuous");
    flxRegister(time, "time", "Time spent converting analog values from internal sensors");
    flxRegister(range, "range", "Range of light being sensed");

    _cacheRange = RANGE_36LUX;
    _cacheTime = CONVERSION_TIME_200MS;
    _cacheMode = OPERATION_MODE_CONTINUOUS;
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevOPT4048::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t tempVal[2] = {0};
    bool retVal =
        i2cDriver.readRegisterRegion(address, SFE_OPT4048_REGISTER_DEVICE_ID, tempVal, 2); // Should return 0x2048

    if (!retVal)
        return false;

    // some bit field magic from the Arduino driver.

    opt4048_reg_device_id_t idReg;

    idReg.word = tempVal[0] << 8;
    idReg.word |= tempVal[1];

    uint16_t chipID = (idReg.DIDH << 2) | idReg.DIDL;

    return (chipID == OPT4048_DEVICE_ID);
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
    bool status = SparkFun_OPT4048::begin(wirePort, address());

    // success?
    if (status)
    {
        // set our initial property values...
        // set our system init value  - normally done automatically when this method returns.
        // But since we're calling our prop methods (who check this value), set it here
        this->setIsInitialized(true);

        set_range(_cacheRange);
        set_conversion_time(_cacheTime);
        set_operation_mode(_cacheMode);
    }
    // Call the Arduino library begin method...
    return status;
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

uint32_t flxDevOPT4048::get_range()
{
    return static_cast<uint>(QwOpt4048::getRange());
}

void flxDevOPT4048::set_range(uint32_t inRange)
{
    if (isInitialized())
        QwOpt4048::setRange(static_cast<opt4048_range_t>(inRange));
    else
        _cacheRange = inRange;
}

uint32_t flxDevOPT4048::get_conversion_time()
{
    return static_cast<uint>(QwOpt4048::getConversionTime());
}

void flxDevOPT4048::set_conversion_time(uint32_t inTime)
{
    if (isInitialized())
        QwOpt4048::setConversionTime(static_cast<opt4048_conversion_time_t>(inTime));
    else
        _cacheTime = inTime;
}

uint32_t flxDevOPT4048::get_operation_mode()
{
    return static_cast<uint>(QwOpt4048::getOperationMode());
}

void flxDevOPT4048::set_operation_mode(uint32_t inMode)
{
    if (isInitialized())
        QwOpt4048::setOperationMode(static_cast<opt4048_operation_mode_t>(inMode));
    else
        _cacheMode = inMode;
}
