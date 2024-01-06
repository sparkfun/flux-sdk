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
 *  flxDevSTC31.h
 *
 *  Spark Device object for the STC31 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_STC3x_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kSTC31DeviceName "STC31"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevSTC31 : public flxDeviceI2CType<flxDevSTC31>, public STC3x
{

  public:
    flxDevSTC31();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kSTC31DeviceName;
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
    float read_co2();
    float read_temperature_C();

    // flags to avoid calling measureGasConcentration multiple times
    bool _co2 = false;
    bool _tempC = false;

    // methods used to set our input parameters
    void write_rh(const float &rh);
    void write_temperature(const float &temperature);
    void write_pressure(const uint &pressure);

    // read-write properties
    uint8_t get_binary_gas();
    void set_binary_gas(uint8_t gas);
    uint8_t _binaryGas = (uint8_t)STC3X_BINARY_GAS_CO2_AIR_25;

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevSTC31, &flxDevSTC31::read_co2> co2;
    flxParameterOutFloat<flxDevSTC31, &flxDevSTC31::read_temperature_C> temperatureC;

    // Define our input parameters
    flxParameterInFloat<flxDevSTC31, &flxDevSTC31::write_rh> rh;
    flxParameterInFloat<flxDevSTC31, &flxDevSTC31::write_temperature> temperatureC_In;
    flxParameterInUint<flxDevSTC31, &flxDevSTC31::write_pressure> pressure;

    // Define our read-write properties
    // binaryGas is STC3X_binary_gas_type_e. Default is STC3X_BINARY_GAS_CO2_AIR_25
    flxPropertyRWUint8<flxDevSTC31, &flxDevSTC31::get_binary_gas, &flxDevSTC31::set_binary_gas> binaryGas = {
        STC3X_BINARY_GAS_CO2_AIR_25,
        {{"CO2 in N2 (100% max)", STC3X_BINARY_GAS_CO2_N2_100},
         {"CO2 in Air (100% max)", STC3X_BINARY_GAS_CO2_AIR_100},
         {"CO2 in N2 (25% max)", STC3X_BINARY_GAS_CO2_N2_25},
         {"CO2 in Air (25% max)", STC3X_BINARY_GAS_CO2_AIR_25}}};
};
