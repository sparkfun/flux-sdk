/*
 *
 *  flxDevMS5637.h
 *
 *  Spark Device object for the MS5637 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_MS5637_Arduino_Library.h"
#include "flxDevice.h"

#define kMS5637DeviceName "MS5637"

// Define our class
class flxDevMS5637 : public flxDeviceI2CType<flxDevMS5637>, public MS5637
{

  public:
    flxDevMS5637();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kMS5637DeviceName;
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

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevMS5637, &flxDevMS5637::read_pressure_mbar> pressure_mbar;    
    flxParameterOutFloat<flxDevMS5637, &flxDevMS5637::read_temperature_C> temperatureC;    
};
