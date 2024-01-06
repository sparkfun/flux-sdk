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
 * QwiicDevMMC5983.cpp
 *
 *  Device object for the MMC5983 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevMMC5983.h"

#define kMMC5983AddressDefault 0x30

// Define our class static variables - allocs storage for them

uint8_t flxDevMMC5983::defaultDeviceAddress[] = {kMMC5983AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevMMC5983);

//----------------------------------------------------------------------------------------------------------
// Base class implementation
//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevMMC5983Base::flxDevMMC5983Base()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(kMMC5983DeviceName, "MMC5983 Magnetometer");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    flxRegister(magX, "X Field (Gauss)", "The X field strength in Gauss");
    flxRegister(magY, "Y Field (Gauss)", "The Y field strength in Gauss");
    flxRegister(magZ, "Z Field (Gauss)", "The Z field strength in Gauss");
    magX.setPrecision(5);
    magY.setPrecision(5);
    magZ.setPrecision(5);
    flxRegister(temperature, "Temperature (C)", "The ambient temperature in degrees C");

    // Register properties
    flxRegister(filterBandwidth, "Filter Bandwidth (Hz)", "The filter bandwidth in Hz");
    flxRegister(autoReset, "Auto-Reset", "Auto-Reset");
}

bool flxDevMMC5983Base::onInitialize(void)
{
    bool result = true;
    result &= SFE_MMC5983MA::softReset();
    result &= SFE_MMC5983MA::setFilterBandwidth(_filter_bandwidth);
    if (_auto_reset)
        result &= SFE_MMC5983MA::enableAutomaticSetReset();
    else
        result &= SFE_MMC5983MA::disableAutomaticSetReset();

    return result;
}

// GETTER methods for output params
double flxDevMMC5983Base::read_x()
{
    if (!_magX)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magY = true;
        _magZ = true;
    }
    _magX = false;
    return ((double)_rawX - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
double flxDevMMC5983Base::read_y()
{
    if (!_magY)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magX = true;
        _magZ = true;
    }
    _magY = false;
    return ((double)_rawY - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
double flxDevMMC5983Base::read_z()
{
    if (!_magZ)
    {
        SFE_MMC5983MA::getMeasurementXYZ(&_rawX, &_rawY, &_rawZ);
        _magX = true;
        _magY = true;
    }
    _magZ = false;
    return ((double)_rawZ - 131072.0) * 8.0 / 131072.0; // Convert to Gauss
}
int flxDevMMC5983Base::read_temperature()
{
    return SFE_MMC5983MA::getTemperature();
}

uint16_t flxDevMMC5983Base::get_filter_bandwidth()
{
    if (isInitialized())
        return SFE_MMC5983MA::getFilterBandwith();
    else
        return _filter_bandwidth;
}
void flxDevMMC5983Base::set_filter_bandwidth(uint16_t bw)
{
    _filter_bandwidth = bw;
    if (isInitialized())
        SFE_MMC5983MA::setFilterBandwidth(bw);
}
uint8_t flxDevMMC5983Base::get_auto_reset()
{
    if (isInitialized())
        return (SFE_MMC5983MA::isAutomaticSetResetEnabled() ? 1 : 0);
    else
        return (uint8_t)_auto_reset;
}
void flxDevMMC5983Base::set_auto_reset(uint8_t enable)
{
    _auto_reset = (bool)enable;
    if (isInitialized())
    {
        if (enable)
            SFE_MMC5983MA::enableAutomaticSetReset();
        else
            SFE_MMC5983MA::disableAutomaticSetReset();
    }
}

//----------------------------------------------------------------------------------------------------------
// I2C Implementation
//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevMMC5983::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t devID = 0;
    bool couldBe5983 = i2cDriver.readRegister(address, PROD_ID_REG, &devID);
    couldBe5983 &= devID == PROD_ID;
    return couldBe5983;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMMC5983::onInitialize(TwoWire &wirePort)
{

    bool result = SFE_MMC5983MA::begin(wirePort);
    if (result)
        result = flxDevMMC5983Base::onInitialize();
    else
        flxLog_E("MMC5983 onInitialize: device did not begin");

    return result;
}

//----------------------------------------------------------------------------------------------------------
// SPI Implementation
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMMC5983_SPI::onInitialize(SPIClass &spiPort)
{

    bool result = SFE_MMC5983MA::begin(chipSelect(), spiPort);

    // begin calls isConnected internally - wh8ich reads the Product ID Register.
    // Occasionally, reading the Product ID Register returns 0xFF instead of the expected 0x30.
    // Not sure why? Possibly something to do with the "OTP Read"?
    // The solution is to do a softReset and then repeat the begin.
    if (!result)
    {
        flxLog_W("MMC5983_SPI onInitialize: device did not begin (1st try). Attempting a softReset");
        SFE_MMC5983MA::softReset();
        result = SFE_MMC5983MA::begin(chipSelect(), spiPort);
    }

    if (result)
        result = flxDevMMC5983Base::onInitialize();
    else
        flxLog_E("MMC5983_SPI onInitialize: device did not begin (2nd try)");

    return result;
}