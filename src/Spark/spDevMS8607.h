/*
 *
 *  spDevMS8607.h
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
class spDevMS8607 : public flxDeviceI2CType<spDevMS8607>, public MS8607
{

  public:
    spDevMS8607();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

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
    flxParameterOutFloat<spDevMS8607, &spDevMS8607::read_pressure_mbar> pressure_mbar;    
    flxParameterOutFloat<spDevMS8607, &spDevMS8607::read_temperature_C> temperatureC;    
    flxParameterOutFloat<spDevMS8607, &spDevMS8607::read_humidity> humidity;    
};
