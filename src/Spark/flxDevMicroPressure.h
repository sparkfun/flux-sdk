/*
 *
 *  flxDevMicroPressure.h
 *
 *  Spark Device object for the MicroPressure device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_MicroPressure.h"
#include "flxDevice.h"

#define kMicroPressureDeviceName "MicroPressure"

// Define our class
class flxDevMicroPressure : public flxDeviceI2CType<flxDevMicroPressure>, public SparkFun_MicroPressure
{

  public:
    flxDevMicroPressure();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kMicroPressureDeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_pressure_PSI();
    float read_pressure_Pa();
    float read_pressure_kPa();
    float read_pressure_torr();
    float read_pressure_inHg();
    float read_pressure_atm();
    float read_pressure_bar();

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_PSI> pressure_PSI;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_Pa> pressure_Pa;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_kPa> pressure_kPa;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_torr> pressure_torr;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_inHg> pressure_inHg;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_atm> pressure_atm;    
    flxParameterOutFloat<flxDevMicroPressure, &flxDevMicroPressure::read_pressure_bar> pressure_bar;    
};
