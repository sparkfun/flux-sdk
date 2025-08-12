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
 *  flxDevBMV080.h
 *
 *  Spark Device object for the BMV080 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_BMV080_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kBMV080DeviceName "BMV080"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBMV080 : public flxDeviceI2CType<flxDevBMV080>, public SparkFunBMV080
{

  public:
    flxDevBMV080();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidencePing;
    }

    static const char *getDeviceName()
    {
        return kBMV080DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // called before reading values
    bool execute(void);

  private:
    // Define our output parameters - these are the values we will return

    float read_pm10(void)
    {
        return SparkFunBMV080::PM10();
    }
    float read_pm25(void)
    {
        return SparkFunBMV080::PM25();
    }
    float read_pm1(void)
    {
        return SparkFunBMV080::PM1();
    }
    bool is_obstructed(void)
    {
        return SparkFunBMV080::isObstructed();
    }

    // Property Getters and Setters
    bool get_enable_obstructed(void);
    // internal method
    void _set_enable_obstructed(bool enable, bool force = false);
    void set_enable_obstructed(bool);

    bool _obstructedEnabled; // Flag to enable/disable obstructed detection

  public:
    // Properties
    flxPropertyRWBool<flxDevBMV080, &flxDevBMV080::get_enable_obstructed, &flxDevBMV080::set_enable_obstructed>
        enableObstructed;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm10> PM10;
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm25> PM25;
    flxParameterOutFloat<flxDevBMV080, &flxDevBMV080::read_pm1> PM1;
    flxParameterOutBool<flxDevBMV080, &flxDevBMV080::is_obstructed> obstructed;
};
