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
 *  flxDevMS8607.h
 *
 *  Spark Device object for the MS8607 device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_PHT_MS8607_Arduino_Library.h"
#include "flxDevice.h"

#define kMS8607DeviceName "MS8607"

// Define our class
class flxDevMS8607 : public flxDeviceI2CType<flxDevMS8607>, public MS8607
{

  public:
    flxDevMS8607();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kMS8607DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_pressure_mbar();
    float read_temperature_C();
    float read_humidity();

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevMS8607, &flxDevMS8607::read_pressure_mbar> pressure_mbar;
    flxParameterOutFloat<flxDevMS8607, &flxDevMS8607::read_temperature_C> temperatureC;
    flxParameterOutFloat<flxDevMS8607, &flxDevMS8607::read_humidity> humidity;
};
