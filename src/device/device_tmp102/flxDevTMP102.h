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
 *  flxDevTMP102.h
 *
 *  Spark Device object for the TMP102 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFunTMP102.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kTMP102DeviceName "TMP102"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevTMP102 : public flxDeviceI2CType<flxDevTMP102>, public TMP102
{

  public:
    flxDevTMP102();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kTMP102DeviceName;
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
    float read_temperature_C();
    float read_temperature_F();

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevTMP102, &flxDevTMP102::read_temperature_C> temperatureC;
    flxParameterOutFloat<flxDevTMP102, &flxDevTMP102::read_temperature_F> temperatureF;
};
