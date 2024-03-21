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
 *  flxDevSGP30.h
 *
 *  Spark Device object for the SGP30 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_SGP30_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kSGP30DeviceName "SGP30"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevSGP30 : public flxDeviceI2CType<flxDevSGP30>, public SGP30
{

  public:
    flxDevSGP30();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kSGP30DeviceName;
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
    uint32_t read_tvoc();
    uint32_t read_co2();
    uint32_t read_h2();
    uint32_t read_ethanol();

    // flags to avoid calling readM<easurement multiple times
    bool _tvoc = false;
    bool _co2 = false;
    bool _h2 = false;
    bool _ethanol = false;

  public:
    // TO DO: add baselineCO2 and baseline TVOC as RW properties

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt32<flxDevSGP30, &flxDevSGP30::read_tvoc> tvoc;
    flxParameterOutUInt32<flxDevSGP30, &flxDevSGP30::read_co2> co2;
    flxParameterOutUInt32<flxDevSGP30, &flxDevSGP30::read_h2> h2;
    flxParameterOutUInt32<flxDevSGP30, &flxDevSGP30::read_ethanol> ethanol;
};
