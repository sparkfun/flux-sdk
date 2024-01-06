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
 *  flxDevSGP40.h
 *
 *  Spark Device object for the SGP40 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_SGP40_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kSGP40DeviceName "SGP40"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevSGP40 : public flxDeviceI2CType<flxDevSGP40>, public SGP40
{

  public:
    flxDevSGP40();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kSGP40DeviceName;
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
    int read_voc();

    void write_rh(const float &rh);
    void write_temperature(const float &temperature);
    float _RH = 50.0;
    float _temperature = 25.0;

  public:
    // Define our input parameters
    flxParameterInFloat<flxDevSGP40, &flxDevSGP40::write_rh> rh;
    flxParameterInFloat<flxDevSGP40, &flxDevSGP40::write_temperature> temperature;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutInt<flxDevSGP40, &flxDevSGP40::read_voc> vocIndex;
};
