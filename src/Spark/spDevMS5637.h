/*
 *
 *  spDevMS5637.h
 *
 *  Spark Device object for the MS5637 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_MS5637_Arduino_Library.h"
#include "spDevice.h"

#define kMS5637DeviceName "MS5637"

// Define our class
class spDevMS5637 : public spDeviceType<spDevMS5637>, public MS5637
{

  public:
    spDevMS5637();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

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
    spParameterOutFloat<spDevMS5637, &spDevMS5637::read_pressure_mbar> pressure_mbar;    
    spParameterOutFloat<spDevMS5637, &spDevMS5637::read_temperature_C> temperatureC;    
};
