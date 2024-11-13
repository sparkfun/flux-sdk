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
 *  flxDevSHTC3.h
 *
 *  Spark Device object for the SHTC3 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevSHTC3.h"

// Define our class static variables - allocs storage for them

#define kSHTC3AddressDefault SHTC3_ADDR_7BIT

uint8_t flxDevSHTC3::defaultDeviceAddress[] = {kSHTC3AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevSHTC3);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevSHTC3::flxDevSHTC3()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SHTC3 Humidity and Temperature Sensor");

    // Register parameters
    flxRegister(humidity, "Humidity (%RH)", "The relative humidity in %", kParamValueHumidity_F);
    flxRegister(temperatureC, "Temperature (C)", "The temperature in degrees C", kParamValueTempC);
    flxRegister(temperatureF, "Temperature (F)", "The temperature in degrees F", kParamValueTempF);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevSHTC3::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t wake[2] = {(uint8_t)(SHTC3_CMD_WAKE >> 8), (uint8_t)(SHTC3_CMD_WAKE & 0xFF)};
    if (!i2cDriver.write(address, wake, 2))
        return false;

    delay(1);

    uint8_t readID[2] = {(uint8_t)(SHTC3_CMD_READ_ID >> 8), (uint8_t)(SHTC3_CMD_READ_ID & 0xFF)};
    if (!i2cDriver.write(address, readID, 2))
        return false;

    uint8_t response[3]; // Two bytes plus CRC
    if (i2cDriver.receiveResponse(address, response, 3) != 3)
        return false;

    // Check the CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 0; x < 2; x++)
    {
        crc ^= response[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31); // x^8+x^5+x^4+1 = 0x31
            else
                crc <<= 1;
        }
    }

    return ((crc == response[2]) && (response[0] & 0b00001000) && ((response[1] & 0b00111111) == 0b00000111));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSHTC3::onInitialize(TwoWire &wirePort)
{

    return (SHTC3::begin(wirePort) == SHTC3_Status_Nominal);
}

// GETTER methods for output params
float flxDevSHTC3::read_humidity()
{
    if (_rh == false)
    {
        SHTC3::update();
        _tempC = true;
        _tempF = true;
    }
    _rh = false;
    return (SHTC3::toPercent());
}

float flxDevSHTC3::read_temperature_C()
{
    if (_tempC == false)
    {
        SHTC3::update();
        _rh = true;
        _tempF = true;
    }
    _tempC = false;
    return (SHTC3::toDegC());
}

float flxDevSHTC3::read_temperature_F()
{
    if (_tempF == false)
    {
        SHTC3::update();
        _rh = true;
        _tempC = true;
    }
    _tempF = false;
    return (SHTC3::toDegF());
}
