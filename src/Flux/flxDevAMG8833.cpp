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
 *  flxDevAMG8833.cpp
 *
 *  Spark Device object for the AMG8833 device.
 *
 *
 */

#include "Arduino.h"

#include "flxDevAMG8833.h"

uint8_t flxDevAMG8833::defaultDeviceAddress[] = {0x69, 0x68, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevAMG8833);

flxDevAMG8833::flxDevAMG8833()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("AMG8833 Grid-EYE infrared array");

    // Register output params
    flxRegister(deviceTemperatureC, "Device Temperature (C)", "The device temperature in degrees C");
    flxRegister(pixelTemperatures, "Pixel Temperatures (C)", "The 64 pixel temperatures in degrees C");
    pixelTemperatures.setPrecision(2);

    // Register property
    flxRegister(frameRate, "Frame Rate (FPS)", "Frame Rate (Frames Per Second)");
}

// Static method used to determine if this device is connected

bool flxDevAMG8833::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{

    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Note - this device could conflict with the SEN54 -- so confidence value of FUZZY is set for this device

    // Read the thermistor temperature. Make sure it is within bounds.
    // Not a great test, but something...
    union {
        int16_t signed16;
        uint16_t unsigned16;
    } signedUnsigned16;
    if (!i2cDriver.readRegister16(address, 0x0E, &signedUnsigned16.unsigned16, true)) // Little-endian
        return false;

    if (signedUnsigned16.unsigned16 >> 12) // Check the 4 unused bits are zero
        return false;

    if (signedUnsigned16.unsigned16 & (1 << 11)) // If the signed bit (bit 11) is set
        signedUnsigned16.unsigned16 |= 0xF000;   // Set the other MS bits to 1 to preserve the two's complement

    // Temperature LSB resolution is 0.0625C. -20C is -320. +60C is 960.
    return ((signedUnsigned16.signed16 > -320) && (signedUnsigned16.signed16 < 960));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevAMG8833::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    GridEYE::begin(address(), wirePort); // Returns void

    if (_frameRate10FPS)
        GridEYE::setFramerate10FPS();
    else
        GridEYE::setFramerate1FPS();

    bool is10FPS = false;
    bool result = GridEYE::getFramerate(&is10FPS); // Checks I2C is working
    result &= is10FPS == _frameRate10FPS;          // Not a great test, but something...

    if (!result)
        flxLog_E("AMG8833 - begin failed");

    return result;
}

// Read-write properties
uint8_t flxDevAMG8833::get_frame_rate()
{
    return (uint8_t)_frameRate10FPS;
}
void flxDevAMG8833::set_frame_rate(uint8_t is10FPS)
{
    _frameRate10FPS = (bool)is10FPS;
    if (isInitialized())
    {
        if (_frameRate10FPS)
            GridEYE::setFramerate10FPS();
        else
            GridEYE::setFramerate1FPS();
    }
}

// Output parameters
float flxDevAMG8833::read_device_temperature_C()
{
    return GridEYE::getDeviceTemperature();
}

bool flxDevAMG8833::read_pixel_temperatures(flxDataArrayFloat *temps)
{
    static float theTemps[64] = {-99.0};

    int i = 0;
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 64; x += 8)
            theTemps[i++] = GridEYE::getPixelTemperature(x + y);

    temps->set(theTemps, 8, 8, true); // don't copy

    return true;
}
