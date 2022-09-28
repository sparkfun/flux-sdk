/*
 *
 * spDevCCS811.cpp
 *
 *  Spark Device object for the CCS811 Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFunCCS811.h"
#include "spDevice.h"



#define kCCS811DeviceName "ccs811"

// Define our class
class spDevCCS811 : public spDeviceType<spDevCCS811>, public CCS811
{

  public:
    spDevCCS811();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kCCS811DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_CO2();
    float read_TVOC();

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevCCS811, &spDevCCS811::read_CO2> co2;
    spParameterOutFloat<spDevCCS811, &spDevCCS811::read_TVOC> tvoc;    

    // Type testing:
    static spType Type;
    spType *getType(void)
    {
        return &Type;
    }
};
