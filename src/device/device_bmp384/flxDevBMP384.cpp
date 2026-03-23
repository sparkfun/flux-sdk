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
 *  flxDevBMP384.cpp
 *
 *  Device object for the BMP384 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBMP384.h"

#define BMP384_CHIP_ID_REG 0x00 // Chip ID 0x50

#define kBMP384AddressDefault 0x77
#define kBMP384AddressAlt1 0x76

// Define our class static variables - allocs storage for them

uint8_t flxDevBMP384::defaultDeviceAddress[] = {kBMP384AddressDefault, kBMP384AddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBMP384);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBMP384::flxDevBMP384()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BMP384 Pressure and Temperature Sensor");

    // Register parameters
    flxRegister(temperatureC, "Temperature (C)", "The sensed temperature in degrees Celsius", kParamValueTempC_D);
    flxRegister(pressure, "Pressure (Pa)", "The sensed pressure in Pascals", kParamValuePressure_D);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBMP384::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = 0;
    bool couldBe384 = i2cDriver.readRegister(address, BMP384_CHIP_ID_REG, &chipID); // Should return 0x50 for BMP384
    couldBe384 &= chipID == 0x50;                                                   // Note: BMP390 returns 0x60

    return (couldBe384);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBMP384::onInitialize(TwoWire &wirePort)
{

    int8_t result = BMP384::beginI2C(address(), wirePort);

    if (result != BMP3_OK)
    {
        flxLog_E("BMP384 Sensor error: %d", result);
        return false;
    }

    return true;
}

// GETTER methods for output params
double flxDevBMP384::read_TemperatureC()
{
    if (!_temperature)
    {
        int8_t err = BMP384::getSensorData(&bmpData);

        if (err == BMP3_OK)
            _pressure = true;
    }
    _temperature = false;
    return bmpData.temperature;
}
double flxDevBMP384::read_Pressure()
{
    if (!_pressure)
    {
        int8_t err = BMP384::getSensorData(&bmpData);

        if (err == BMP3_OK)
            _temperature = true;
    }
    _pressure = false;
    return bmpData.pressure;
}
