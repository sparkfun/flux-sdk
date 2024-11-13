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
 *  flxDevAHT20.h
 *
 *  Spark Device object for the AHT20 device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_Qwiic_Humidity_AHT20.h"
#include "flxDevice.h"

#define kAHT20DeviceName "AHT20"

// Define our class
class flxDevAHT20 : public flxDeviceI2CType<flxDevAHT20>, public AHT20
{

  public:
    flxDevAHT20();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kAHT20DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_temperature_c();
    float read_humidity();

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevAHT20, &flxDevAHT20::read_temperature_c> temperatureC;
    flxParameterOutFloat<flxDevAHT20, &flxDevAHT20::read_humidity> humidity;
};
