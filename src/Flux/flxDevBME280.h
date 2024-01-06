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
 *  flxDevBME280.h
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFunBME280.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kBME280DeviceName "BME280"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBME280 : public flxDeviceI2CType<flxDevBME280>, public BME280
{

  public:
    flxDevBME280();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kBME280DeviceName;
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
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_Humidity> humidity;
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_TemperatureF> temperatureF;
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_TemperatureC> temperatureC;
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_Pressure> pressure;
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_AltitudeM> altitudeM;
    flxParameterOutFloat<flxDevBME280, &flxDevBME280::read_AltitudeF> altitudeF;
};
