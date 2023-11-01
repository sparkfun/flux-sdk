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
 *  flxDevOPT4048.h
 *
 *  Device object for the OPT4048 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFunOPT4048.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kOPT4048DeviceName "OPT4048"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevOPT4048 : public flxDeviceI2CType<flxDevOPT4048>, public OPT4048
{

  public:
    flxDevOPT4048();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kOPT4048DeviceName;
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
    float read_Humidity();
    float read_TemperatureF();
    float read_TemperatureC();
    float read_Pressure();
    float read_AltitudeM();
    float read_AltitudeF();

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_Humidity> humidity;
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_TemperatureF> temperatureF;
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_TemperatureC> temperatureC;
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_Pressure> pressure;
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_AltitudeM> altitudeM;
    flxParameterOutFloat<flxDevOPT4048, &flxDevOPT4048::read_AltitudeF> altitudeF;
};
