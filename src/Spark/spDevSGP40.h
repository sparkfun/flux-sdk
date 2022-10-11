/*
 *
 *  spDevSGP40.h
 *
 *  Spark Device object for the SGP40 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_SGP40_Arduino_Library.h"

// What is the name used to ID this device?
#define kSGP40DeviceName "sgp40"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSGP40 : public spDeviceType<spDevSGP40>, public SGP40
{

public:
    spDevSGP40();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    spParameterInFloat<spDevSGP40, &spDevSGP40::write_rh> rh;
    spParameterInFloat<spDevSGP40, &spDevSGP40::write_temperature> temperature;

    // Define our output parameters - specify the get functions to call.
    spParameterOutInt<spDevSGP40, &spDevSGP40::read_voc> vocIndex;    
};
