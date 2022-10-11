/*
 *
 *  spDevVEML7700.h
 *
 *  Spark Device object for the VEML7700 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_VEML7700_Arduino_Library.h"

// What is the name used to ID this device?
#define kVEML7700DeviceName "veml7700"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevVEML7700 : public spDeviceType<spDevVEML7700>, public VEML7700
{

public:
    spDevVEML7700();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kVEML7700DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

private:

    // methods used to get values for our output parameters
    uint read_ambient_light();
    uint read_white_level();
    float read_lux();

    // methods to get/set our read-write properties
    uint get_integration_time();
    void set_integration_time(uint);
    uint get_sensitivity();
    void set_sensitivity(uint);
    uint get_persistence();
    void set_persistence(uint);

public:
    // Define our read-write properties
    spPropertyRWUint<spDevVEML7700, &spDevVEML7700::get_integration_time, &spDevVEML7700::set_integration_time> integrationTime;
    spPropertyRWUint<spDevVEML7700, &spDevVEML7700::get_sensitivity, &spDevVEML7700::set_sensitivity> sensitivity;
    spPropertyRWUint<spDevVEML7700, &spDevVEML7700::get_persistence, &spDevVEML7700::set_persistence> persistence;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevVEML7700, &spDevVEML7700::read_ambient_light> ambientLight;    
    spParameterOutUint<spDevVEML7700, &spDevVEML7700::read_white_level> whiteLevel;    
    spParameterOutFloat<spDevVEML7700, &spDevVEML7700::read_lux> lux;    
};
