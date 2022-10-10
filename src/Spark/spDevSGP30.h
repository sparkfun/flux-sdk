/*
 *
 *  spDevSGP30.h
 *
 *  Spark Device object for the SGP30 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_SGP30_Arduino_Library.h"

// What is the name used to ID this device?
#define kSGP30DeviceName "sgp30"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSGP30 : public spDeviceType<spDevSGP30>, public SGP30
{

public:
    spDevSGP30();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSGP30DeviceName;
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
    uint read_tvoc();
    uint read_co2();
    uint read_h2();
    uint read_ethanol();

    // flags to avoid calling readM<easurement multiple times
    bool _tvoc = false;
    bool _co2 = false;
    bool _h2 = false;
    bool _ethanol = false;

public:
    // TO DO: add baselineCO2 and baseline TVOC as RW properties

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevSGP30, &spDevSGP30::read_tvoc> tvoc;    
    spParameterOutUint<spDevSGP30, &spDevSGP30::read_co2> co2;    
    spParameterOutUint<spDevSGP30, &spDevSGP30::read_h2> h2;    
    spParameterOutUint<spDevSGP30, &spDevSGP30::read_ethanol> ethanol;    
};
