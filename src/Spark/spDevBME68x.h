/*
 *
 *  spDevBME68x.h
 *
 *  Device object for the BME68x Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "bme68xLibrary.h"

// What is the name used to ID this device?
#define kBME68xDeviceName "BME68x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBME68x : public spDeviceI2CType<spDevBME68x>, public Bme68x
{

public:
    spDevBME68x();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBME68xDeviceName;
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
	bme68xData bmeData = { 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0 };

    // methods used to get values for our output parameters
    float read_Humidity();
    float read_TemperatureC();
    float read_Pressure();
    float read_GasResistance();
    uint8_t read_Status();

    // flags to prevent setOpMode and fetchData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;
    bool _humidity = false;
    bool _gasResistance = false;
    bool _status = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevBME68x, &spDevBME68x::read_Humidity> humidity;
    spParameterOutFloat<spDevBME68x, &spDevBME68x::read_TemperatureC> temperatureC;
    spParameterOutFloat<spDevBME68x, &spDevBME68x::read_Pressure> pressure;
    spParameterOutFloat<spDevBME68x, &spDevBME68x::read_GasResistance> gasResistance;
    spParameterOutUint8<spDevBME68x, &spDevBME68x::read_Status> status;

};
