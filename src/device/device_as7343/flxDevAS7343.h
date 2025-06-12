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
 *  flxDevAS7343.h
 *
 *  Spark Device object for the AS7343 Qwiic device.
 */

#pragma once
#include "SparkFun_AS7343.h"
#include "flxCore.h"
#include "flxDevice.h"

#define kAS7343DeviceName "AS7343"

// Define our class
class flxDevAS7343 : public flxDeviceI2CType<flxDevAS7343>, public SfeAS7343ArdI2C
{

  public:
    flxDevAS7343();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kAS7343DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects.
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    bool execute(void);

  private:
    // gain pro getter/setter/stash
    uint8_t get_gain(void);
    void set_gain(uint8_t);
    uint8_t _gain;

    // Read Spectra data
    bool get_read_spectra(void);
    void set_read_spectra(bool);
    bool _read_spectra;

    // Flicker detect?
    bool get_flicker_detect(void);
    void set_flicker_detect(bool);
    bool _flicker_detect;

    uint16_t get_blue_value(void)
    {
        return getBlue();
    }
    uint16_t get_green_value(void)
    {
        return getGreen();
    }
    uint16_t get_red_value(void)
    {
        return getRed();
    }
    uint16_t get_nir_value(void)
    {
        return getNIR();
    }
    bool get_spectral_data(flxDataArrayUInt16 *);
    uint8_t get_flicker_value(void);
    bool _valid_data;

  public:
    // our data accessors - use our base class for the simple readings

    // Properties

    // Read with LED active?

    flxPropertyBool<flxDevAS7343> readWithLED = {true};

    flxPropertyRWBool<flxDevAS7343, &flxDevAS7343::get_read_spectra, &flxDevAS7343::set_read_spectra> readSpectra = {
        true};

    flxPropertyRWBool<flxDevAS7343, &flxDevAS7343::get_flicker_detect, &flxDevAS7343::set_flicker_detect>
        flickerDetect = {false};
    // Gain prop
    flxPropertyRWUInt8<flxDevAS7343, &flxDevAS7343::get_gain, &flxDevAS7343::set_gain> sensorGain = {
        AGAIN_256,
        {{"0.5", AGAIN_0_5}, // 0.5x gain
         {"1.0", AGAIN_1},
         {"2.0", AGAIN_2},
         {"4.0", AGAIN_4},
         {"8.0", AGAIN_8},
         {"16.0", AGAIN_16},
         {"32.0", AGAIN_32},
         {"64.0", AGAIN_64},
         {"128.0", AGAIN_128},
         {"256.0", AGAIN_256},
         {"512.0", AGAIN_512},
         {"1024.0", AGAIN_1024},
         {"2048.0", AGAIN_2048}}};

    // Data parameters
    flxParameterOutUInt16<flxDevAS7343, &flxDevAS7343::get_blue_value> blueValue;
    flxParameterOutUInt16<flxDevAS7343, &flxDevAS7343::get_green_value> greenValue;
    flxParameterOutUInt16<flxDevAS7343, &flxDevAS7343::get_red_value> redValue;
    flxParameterOutUInt16<flxDevAS7343, &flxDevAS7343::get_nir_value> nirValue;
    flxParameterOutUInt8<flxDevAS7343, &flxDevAS7343::get_flicker_value> flickerValue;

    flxParameterOutArrayUInt16<flxDevAS7343, &flxDevAS7343::get_spectral_data> spectralData;
};