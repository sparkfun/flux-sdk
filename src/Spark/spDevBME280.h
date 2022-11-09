/*
 *
 *  spDevBME280.h
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFunBME280.h"

// What is the name used to ID this device?
#define kBME280DeviceName "BME280"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBME280 : public spDeviceI2CType<spDevBME280>, public BME280
{

public:
    spDevBME280();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBME280DeviceName;
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
    float read_Humidity();
    float read_TemperatureF();
    float read_TemperatureC();
    float read_Pressure();
    float read_AltitudeM();
    float read_AltitudeF();

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevBME280, &spDevBME280::read_Humidity> humidity;
    spParameterOutFloat<spDevBME280, &spDevBME280::read_TemperatureF> temperatureF;
    spParameterOutFloat<spDevBME280, &spDevBME280::read_TemperatureC> temperatureC;
    spParameterOutFloat<spDevBME280, &spDevBME280::read_Pressure> pressure;
    spParameterOutFloat<spDevBME280, &spDevBME280::read_AltitudeM> altitudeM;
    spParameterOutFloat<spDevBME280, &spDevBME280::read_AltitudeF> altitudeF;

};
