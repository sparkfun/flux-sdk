/*
 *
 *  spDevSTC31.h
 *
 *  Spark Device object for the STC31 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_STC3x_Arduino_Library.h"

// What is the name used to ID this device?
#define kSTC31DeviceName "stc31"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevSTC31 : public spDeviceType<spDevSTC31>, public STC3x
{

public:
    spDevSTC31();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    spParameterOutFloat<spDevSTC31, &spDevSTC31::read_co2> co2;    
    spParameterOutFloat<spDevSTC31, &spDevSTC31::read_temperature_C> temperatureC;    

    // Define our input parameters
    spParameterInFloat<spDevSTC31, &spDevSTC31::write_rh> rh;
    spParameterInFloat<spDevSTC31, &spDevSTC31::write_temperature> temperatureC_In;
    spParameterInUint<spDevSTC31, &spDevSTC31::write_pressure> pressure;

    // Define our read-write properties
    // binaryGas is STC3X_binary_gas_type_e. Default is STC3X_BINARY_GAS_CO2_AIR_25
    spPropertyRWUint8<spDevSTC31, &spDevSTC31::get_binary_gas, &spDevSTC31::set_binary_gas> binaryGas;
    spDataLimitSetUint8 binary_gas_limit = { { "CO2 in N2 (100% max)", STC3X_BINARY_GAS_CO2_N2_100 },
                                             { "CO2 in Air (100% max)", STC3X_BINARY_GAS_CO2_AIR_100 },
                                             { "CO2 in N2 (25% max)", STC3X_BINARY_GAS_CO2_N2_25 },
                                             { "CO2 in Air (25% max)", STC3X_BINARY_GAS_CO2_AIR_25 } };
};
