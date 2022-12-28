/*
 *
 *  flxDevBMP384.h
 *
 *  Device object for the BMP384 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SparkFunBMP384.h"

// What is the name used to ID this device?
#define kBMP384DeviceName "BMP384"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevBMP384 : public flxDeviceI2CType<flxDevBMP384>, public BMP384
{

public:
    flxDevBMP384();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
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
	bmp3_data bmpData = { 0.0, 0.0 };

    // methods used to get values for our output parameters
    double read_TemperatureC();
    double read_Pressure();

    // flags to prevent getSensorData from being called multiple times
    bool _temperature = false;
    bool _pressure = false;

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutDouble<flxDevBMP384, &flxDevBMP384::read_TemperatureC> temperatureC;
    flxParameterOutDouble<flxDevBMP384, &flxDevBMP384::read_Pressure> pressure;

};
