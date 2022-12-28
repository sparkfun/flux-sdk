/*
 *
 *  flxDevMAX17048.h
 *
 *  Spark Device object for the MAX17048 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"
#include "flxDevice.h"

#define kMAX17048DeviceName "MAX17048"

// Define our class
class flxDevMAX17048 : public flxDeviceI2CType<flxDevMAX17048>, public SFE_MAX1704X
{

  public:
    flxDevMAX17048();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kMAX17048DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_voltage();
    float read_state_of_charge();
    float read_change_rate();

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_voltage> voltageV;
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_state_of_charge> stateOfCharge;    
    flxParameterOutFloat<flxDevMAX17048, &flxDevMAX17048::read_change_rate> changeRate;    
};
