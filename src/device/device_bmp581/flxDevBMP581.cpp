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
 *  flxDevBMP581.cpp
 *
 *  Device object for the BMP581 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBMP581.h"

#define BMP581_CHIP_ID_REG 0x01 // Chip ID 0x50

#define kBMP581AddressDefault 0x47
#define kBMP581AddressAlt1 0x46

// Define our class static variables - allocs storage for them

uint8_t flxDevBMP581::defaultDeviceAddress[] = {kBMP581AddressDefault, kBMP581AddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBMP581);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBMP581::flxDevBMP581()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BMP581 Pressure and Temperature Sensor.");

    // Register parameters
    flxRegister(temperatureC, "Temperature (C)", "The sensed temperature in degrees Celsius", kParamValueTempC);
    flxRegister(pressure, "Pressure (Pa)", "The sensed pressure in Pascals", kParamValuePressure_F);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBMP581::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = 0;
    bool couldBe581 = i2cDriver.readRegister(address, BMP581_CHIP_ID_REG, &chipID); // Should return 0x50 for BMP581
    couldBe581 &= chipID == 0x50;                                                   // Note: BMP390 returns 0x60

    return (couldBe581);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBMP581::onInitialize(TwoWire &wirePort)
{

    int8_t result = BMP581::beginI2C(address(), wirePort);

    if (result != BMP5_OK)
    {
        flxLog_E("BMP581 Sensor error: %d", result);
        return false;
    }

    _begun = true;

    BMP581::setMode((bmp5_powermode)_powerMode);

    return true;
}

// GETTER methods for output params
float flxDevBMP581::read_TemperatureC()
{
    if (!_temperature)
    {
        if ((_powerMode == BMP5_POWERMODE_DEEP_STANDBY) || (_powerMode == BMP5_POWERMODE_STANDBY))
        {
            BMP581::setMode(BMP5_POWERMODE_FORCED);
            // Note to self: do we need to delay here? Or poll data ready somehow?
        }

        int8_t err = BMP581::getSensorData(&bmpData);

        if (err == BMP5_OK)
            _pressure = true;
    }
    _temperature = false;
    return bmpData.temperature;
}
float flxDevBMP581::read_Pressure()
{
    if (!_pressure)
    {
        if ((_powerMode == BMP5_POWERMODE_DEEP_STANDBY) || (_powerMode == BMP5_POWERMODE_STANDBY))
        {
            BMP581::setMode(BMP5_POWERMODE_FORCED);
            // Note to self: do we need to delay here? Or poll data ready somehow?
        }

        int8_t err = BMP581::getSensorData(&bmpData);

        if (err == BMP5_OK)
            _temperature = true;
    }
    _pressure = false;
    return bmpData.pressure;
}

// Get-Set methods for our read-write properties
uint8_t flxDevBMP581::get_power_mode()
{
    return _powerMode;
}
void flxDevBMP581::set_power_mode(uint8_t mode)
{
    _powerMode = mode;
    if (_begun)
        BMP581::setMode((bmp5_powermode)mode);
}
