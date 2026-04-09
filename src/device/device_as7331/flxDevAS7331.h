/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2025, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevAS7331.h
 *
 *  Spark Device object for the AS7331 UV Spectral Sensor (UVA/UVB/UVC).
 */

#pragma once
#include "SparkFun_AS7331.h"
#include "flxCore.h"
#include "flxDevice.h"

#define kAS7331DeviceName "AS7331"

// Define our class
class flxDevAS7331 : public flxDeviceI2CType<flxDevAS7331>, public SfeAS7331ArdI2C
{

  public:
    flxDevAS7331();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        // Addresses 0x76/0x77 overlap with BME280/BMP384 — chip ID check resolves this
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kAS7331DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this object
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    bool execute(void);

  private:
    // Gain property getter/setter
    uint16_t get_gain(void);
    void set_gain(uint16_t);
    uint16_t _gain;

    // Conversion time property getter/setter
    uint16_t get_conv_time(void);
    void set_conv_time(uint16_t);
    uint16_t _convTime;

    // UV channel accessors — values cached by execute()
    float get_uva_value(void)
    {
        return _valid_data ? getUVA() : 0.0f;
    }
    float get_uvb_value(void)
    {
        return _valid_data ? getUVB() : 0.0f;
    }
    float get_uvc_value(void)
    {
        return _valid_data ? getUVC() : 0.0f;
    }
    float get_temp_value(void)
    {
        return _valid_data ? getTemp() : 0.0f;
    }

    bool _valid_data;
    bool _in_setup;

  public:
    // Properties

    // Gain setting
    flxPropertyRWUInt16<flxDevAS7331, &flxDevAS7331::get_gain, &flxDevAS7331::set_gain> sensorGain = {
        GAIN_256,
        {{"1", GAIN_1},
         {"2", GAIN_2},
         {"4", GAIN_4},
         {"8", GAIN_8},
         {"16", GAIN_16},
         {"32", GAIN_32},
         {"64", GAIN_64},
         {"128", GAIN_128},
         {"256", GAIN_256},
         {"512", GAIN_512},
         {"1024", GAIN_1024},
         {"2048", GAIN_2048}}};

    // Conversion time setting
    flxPropertyRWUInt16<flxDevAS7331, &flxDevAS7331::get_conv_time, &flxDevAS7331::set_conv_time> conversionTime = {
        TIME_64MS,
        {{"1ms", TIME_1MS},
         {"2ms", TIME_2MS},
         {"4ms", TIME_4MS},
         {"8ms", TIME_8MS},
         {"16ms", TIME_16MS},
         {"32ms", TIME_32MS},
         {"64ms", TIME_64MS},
         {"128ms", TIME_128MS},
         {"256ms", TIME_256MS},
         {"512ms", TIME_512MS},
         {"1024ms", TIME_1024MS},
         {"2048ms", TIME_2048MS},
         {"4096ms", TIME_4096MS},
         {"8192ms", TIME_8192MS},
         {"16384ms", TIME_16384MS}}};

    // Data parameters — UV irradiance in uW/cm²
    flxParameterOutFloat<flxDevAS7331, &flxDevAS7331::get_uva_value> uvaValue;
    flxParameterOutFloat<flxDevAS7331, &flxDevAS7331::get_uvb_value> uvbValue;
    flxParameterOutFloat<flxDevAS7331, &flxDevAS7331::get_uvc_value> uvcValue;
    flxParameterOutFloat<flxDevAS7331, &flxDevAS7331::get_temp_value> temperatureC;
};
