/*
 *
 *  spDevSDP3X.h
 *
 *  Spark Device object for the SDP3X device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_SDP3x_Arduino_Library.h"

// What is the name used to ID this device?
#define kSDP3XDeviceName "SDP3x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSDP3X : public spDeviceI2CType<spDevSDP3X>, public SDP3X
{

public:
    spDevSDP3X();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSDP3XDeviceName;
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
    float read_temperature_C();
    float read_pressure();

    // methods used to get values for our RW properties
    uint8_t get_temperature_compensation();
    uint8_t get_measurement_averaging();
    void set_temperature_compensation(uint8_t);
    void set_measurement_averaging(uint8_t);

    // flags to avoid calling readM<easurement multiple times
    float _temperature = -9999; // Mark temperature as stale
    float _pressure = -9999; // Mark pressure as stale
    uint8_t _tempComp = 1; // Default to mass flow temperature compensation with no averaging
    bool _measAvg = false;

    bool _begun = false;

public:
    spPropertyRWUint8<spDevSDP3X, &spDevSDP3X::get_temperature_compensation, &spDevSDP3X::set_temperature_compensation> temperatureCompensation
        = { 1, { { "Differential Pressure", 0 }, { "Mass Flow", 1 } } };
    spPropertyRWUint8<spDevSDP3X, &spDevSDP3X::get_measurement_averaging, &spDevSDP3X::set_measurement_averaging> measurementAveraging
        = { 0, { { "Disabled", 0 }, { "Enabled", 1 } } };

    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevSDP3X, &spDevSDP3X::read_temperature_C> temperatureC;    
    spParameterOutFloat<spDevSDP3X, &spDevSDP3X::read_pressure> pressure;    
};
