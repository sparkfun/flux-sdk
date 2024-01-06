/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevAMG8833.h
 *
 *  Spark Device object for the AMG8833 device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_GridEYE_Arduino_Library.h"
#include "flxDevice.h"

#define kAMG8833DeviceName "AMG8833"

// Define our class
class flxDevAMG8833 : public flxDeviceI2CType<flxDevAMG8833>, public GridEYE
{

  public:
    flxDevAMG8833();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceFuzzy;
    }

    static const char *getDeviceName()
    {
        return kAMG8833DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_device_temperature_C();
    bool read_pixel_temperatures(flxDataArrayFloat *);

    bool _frameRate10FPS = true; // Default to 10 FPS

    uint8_t get_frame_rate();
    void set_frame_rate(uint8_t);

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevAMG8833, &flxDevAMG8833::read_device_temperature_C> deviceTemperatureC;
    flxParameterOutArrayFloat<flxDevAMG8833, &flxDevAMG8833::read_pixel_temperatures> pixelTemperatures;

    flxPropertyRWUint8<flxDevAMG8833, &flxDevAMG8833::get_frame_rate, &flxDevAMG8833::set_frame_rate> frameRate = {
        1, {{"1 Frame Per Second", 0}, {"10 Frames Per Second", 1}}};
};
