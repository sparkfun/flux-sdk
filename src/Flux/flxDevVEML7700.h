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
 *  flxDevVEML7700.h
 *
 *  Spark Device object for the VEML7700 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFun_VEML7700_Arduino_Library.h"

// What is the name used to ID this device?
#define kVEML7700DeviceName "VEML7700"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevVEML7700 : public flxDeviceI2CType<flxDevVEML7700>, public VEML7700
{

public:
    flxDevVEML7700();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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
    uint8_t get_integration_time();
    void set_integration_time(uint8_t);
    uint8_t get_sensitivity();
    void set_sensitivity(uint8_t);
    uint8_t get_persistence();
    void set_persistence(uint8_t);

    uint8_t _integrationTime = VEML7700_INTEGRATION_100ms;
    uint8_t _sensitivity = VEML7700_SENSITIVITY_x1;
    uint8_t _persistence = VEML7700_PERSISTENCE_1;

public:
    // Define our read-write properties
    flxPropertyRWUint8<flxDevVEML7700, &flxDevVEML7700::get_integration_time, &flxDevVEML7700::set_integration_time> integrationTime
        = { VEML7700_INTEGRATION_100ms, { { "25ms", VEML7700_INTEGRATION_25ms }, { "50ms", VEML7700_INTEGRATION_50ms }, { "100ms", VEML7700_INTEGRATION_100ms },
                                          { "200ms", VEML7700_INTEGRATION_200ms }, { "400ms", VEML7700_INTEGRATION_400ms }, { "800ms", VEML7700_INTEGRATION_800ms } } };
    flxPropertyRWUint8<flxDevVEML7700, &flxDevVEML7700::get_sensitivity, &flxDevVEML7700::set_sensitivity> sensitivity
        = { VEML7700_SENSITIVITY_x1, { { "x1", VEML7700_SENSITIVITY_x1 }, { "x2", VEML7700_SENSITIVITY_x2 },
                                       { "x1/8", VEML7700_SENSITIVITY_x1_8 }, { "x1/4", VEML7700_SENSITIVITY_x1_4 } } };
    flxPropertyRWUint8<flxDevVEML7700, &flxDevVEML7700::get_persistence, &flxDevVEML7700::set_persistence> persistence
        = { VEML7700_PERSISTENCE_1, { { "x1", VEML7700_PERSISTENCE_1 }, { "x2", VEML7700_PERSISTENCE_2 },
                                      { "x4", VEML7700_PERSISTENCE_4 }, { "x8", VEML7700_PERSISTENCE_8 } } };

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUint<flxDevVEML7700, &flxDevVEML7700::read_ambient_light> ambientLight;    
    flxParameterOutUint<flxDevVEML7700, &flxDevVEML7700::read_white_level> whiteLevel;    
    flxParameterOutFloat<flxDevVEML7700, &flxDevVEML7700::read_lux> lux;    
};
