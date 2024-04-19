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
 *  flxDevVCNL4040.h
 *
 *  Spark Device object for the VCNL4040 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_VCNL4040_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kVCNL4040DeviceName "VCNL4040"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevVCNL4040 : public flxDeviceI2CType<flxDevVCNL4040>, public VCNL4040
{

  public:
    flxDevVCNL4040();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kVCNL4040DeviceName;
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
    uint16_t read_proximity();
    uint16_t read_lux();

    // methods to get/set our read-write properties
    uint8_t get_LED_current();
    void set_LED_current(uint8_t);
    uint16_t get_IR_duty_cycle();
    void set_IR_duty_cycle(uint16_t);
    uint8_t get_proximity_integration_time();
    void set_proximity_integration_time(uint8_t);
    uint8_t get_proximity_resolution();
    void set_proximity_resolution(uint8_t);
    uint16_t get_ambient_integration_time();
    void set_ambient_integration_time(uint16_t);

    uint8_t _ledCurrent = 200;
    uint16_t _irDutyCycle = 40;
    uint8_t _proxIntTime = 8;
    uint8_t _proxRes = 16; // Default to 16-bit
    uint16_t _ambIntTime = 80;

  public:
    // Define our read-write properties
    flxPropertyRWUInt8<flxDevVCNL4040, &flxDevVCNL4040::get_LED_current, &flxDevVCNL4040::set_LED_current> ledCurrent =
        {50, 200};

    flxPropertyRWUInt16<flxDevVCNL4040, &flxDevVCNL4040::get_IR_duty_cycle, &flxDevVCNL4040::set_IR_duty_cycle>
        irDutyCycle = {40, 320};

    flxPropertyRWUInt8<flxDevVCNL4040, &flxDevVCNL4040::get_proximity_integration_time,
                       &flxDevVCNL4040::set_proximity_integration_time>
        proximityIntegrationTime = {1, 8};

    flxPropertyRWUInt8<flxDevVCNL4040, &flxDevVCNL4040::get_proximity_resolution,
                       &flxDevVCNL4040::set_proximity_resolution>
        proximityResolution = {16, {{"12-bit", 12}, {"16-bit", 16}}};

    flxPropertyRWUInt16<flxDevVCNL4040, &flxDevVCNL4040::get_ambient_integration_time,
                        &flxDevVCNL4040::set_ambient_integration_time>
        ambientIntegrationTime = {80, 640};

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt16<flxDevVCNL4040, &flxDevVCNL4040::read_proximity> proximity;
    flxParameterOutUInt16<flxDevVCNL4040, &flxDevVCNL4040::read_lux> lux;
};
