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
 *  flxDevCCS811.h
 *
 *  Spark Device object for the CCS811 Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFunCCS811.h"
#include "flxDevice.h"

#define kCCS811DeviceName "CCS811"

// Define our class
class flxDevCCS811 : public flxDeviceI2CType<flxDevCCS811>, public CCS811
{

  public:
    flxDevCCS811();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kCCS811DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_CO2();
    float read_TVOC();
    bool _tvoc = false; // Flags to avoid calling readAlgorithmResults twice
    bool _co2 = false;

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevCCS811, &flxDevCCS811::read_CO2> co2;
    flxParameterOutFloat<flxDevCCS811, &flxDevCCS811::read_TVOC> tvoc;
};
