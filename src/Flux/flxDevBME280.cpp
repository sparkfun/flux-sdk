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
 *  flxDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevBME280.h"

#define BME280_CHIP_ID_REG 0xD0 // Chip ID

#define kBMEAddressDefault 0x77
#define kBMEAddressAlt1 0x76

// Define our class static variables - allocs storage for them

uint8_t flxDevBME280::defaultDeviceAddress[] = {kBMEAddressDefault, kBMEAddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevBME280);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevBME280::flxDevBME280()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BME280 Atmospheric Sensor");

    // Register parameters
    flxRegister(humidity, "Humidity", "The sensed humidity value");
    flxRegister(temperatureF, "TemperatureF", "The sensed Temperature in degrees Fahrenheit");
    flxRegister(temperatureC, "TemperatureC", "The sensed Temperature in degrees Celsius");
    flxRegister(pressure, "Pressure", "The sensed pressure");
    flxRegister(altitudeM, "AltitudeM", "The sensed altitude in meters");
    flxRegister(altitudeF, "AltitudeF", "The sensed altitude in feet");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevBME280::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = i2cDriver.readRegister(address, BME280_CHIP_ID_REG); // Should return 0x60 or 0x58

    //flxLog_I("BME280 isConnected chip ID 0x%02x", chipID);

    return (chipID == 0x58 || chipID == 0x60);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevBME280::onInitialize(TwoWire &wirePort)
{

    // set the device address
    BME280::setI2CAddress(address());
    return BME280::beginI2C(wirePort);
}

// GETTER methods for output params
float flxDevBME280::read_Humidity() { return BME280::readFloatHumidity(); }

float flxDevBME280::read_TemperatureF() { return BME280::readTempF(); }
float flxDevBME280::read_TemperatureC() { return BME280::readTempC(); }

float flxDevBME280::read_Pressure() { return BME280::readFloatPressure(); }

float flxDevBME280::read_AltitudeM() { return BME280::readFloatAltitudeMeters(); }
float flxDevBME280::read_AltitudeF() { return BME280::readFloatAltitudeFeet(); }
