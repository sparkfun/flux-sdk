/*
 *
 *  spDevSHTC3.h
 *
 *  Spark Device object for the SHTC3 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFun_SHTC3.h"

// What is the name used to ID this device?
#define kSHTC3DeviceName "SHTC3"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSHTC3 : public flxDeviceI2CType<spDevSHTC3>, public SHTC3
{

public:
    spDevSHTC3();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSHTC3DeviceName;
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
    float read_humidity();
    float read_temperature_C();
    float read_temperature_F();

    // flags to avoid calling update multiple times
    bool _rh = false;
    bool _tempC = false;
    bool _tempF = false;

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<spDevSHTC3, &spDevSHTC3::read_humidity> humidity;    
    flxParameterOutFloat<spDevSHTC3, &spDevSHTC3::read_temperature_C> temperatureC;    
    flxParameterOutFloat<spDevSHTC3, &spDevSHTC3::read_temperature_F> temperatureF;    
};
