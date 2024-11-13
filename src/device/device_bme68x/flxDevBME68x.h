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
 *  flxDevBME68x.h
 *
 *  Device object for the BME68x Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "bme68xLibrary.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kBME68xDeviceName "BME68x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBME68x : public flxDeviceI2CType<flxDevBME68x>, public Bme68x
{

  public:
    flxDevBME68x();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kBME68xDeviceName;
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
    bme68xData bmeData = {0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0};

    // methods used to get values for our output parameters
    float read_Humidity();
    float read_TemperatureC();
    float read_Pressure();
    float read_GasResistance();
    uint8_t read_Status();

    // flags to prevent setOpMode and fetchData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;
    bool _humidity = false;
    bool _gasResistance = false;
    bool _status = false;

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevBME68x, &flxDevBME68x::read_Humidity> humidity;
    flxParameterOutFloat<flxDevBME68x, &flxDevBME68x::read_TemperatureC> temperatureC;
    flxParameterOutFloat<flxDevBME68x, &flxDevBME68x::read_Pressure> pressure;
    flxParameterOutFloat<flxDevBME68x, &flxDevBME68x::read_GasResistance> gasResistance;
    flxParameterOutUInt8<flxDevBME68x, &flxDevBME68x::read_Status> status;
};
