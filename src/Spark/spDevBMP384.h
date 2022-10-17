/*
 *
 *  spDevBMP384.h
 *
 *  Device object for the BMP384 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFunBMP384.h"

// What is the name used to ID this device?
#define kBMP384DeviceName "bmp384"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevBMP384 : public spDeviceType<spDevBMP384>, public BMP384
{

public:
    spDevBMP384();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kBMP384DeviceName;
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
	bmp3_data bmpData;

    // methods used to get values for our output parameters
    double read_TemperatureC();
    double read_Pressure();

    // flags to prevent setOpMode and fetchData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutDouble<spDevBMP384, &spDevBMP384::read_TemperatureC> temperatureC;
    spParameterOutDouble<spDevBMP384, &spDevBMP384::read_Pressure> pressure;

};
