/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 * 
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevADS1015.h
 *
 *  Spark Device object for the ADS1015 A/D converter device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_ADS1015_Arduino_Library.h"
#include "flxDevice.h"



#define kADS1015DeviceName "ADS1015"

// Define our class
class flxDevADS1015 : public flxDeviceI2CType<flxDevADS1015>, public ADS1015
{

  public:
    flxDevADS1015();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kADS1015DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_single_0();
    float read_single_1();
    float read_single_2();
    float read_single_3();
    float read_differential_P0_N1();
    float read_differential_P0_N3();
    float read_differential_P1_N3();
    float read_differential_P2_N3();

    uint16_t get_sample_rate();
    void set_sample_rate(uint16_t);
    uint16_t get_pga_gain();
    void set_pga_gain(uint16_t);

    uint16_t _sampleRate = ADS1015_CONFIG_RATE_1600HZ;
    uint16_t _gain = ADS1015_CONFIG_PGA_2;

public:
    flxPropertyRWUint16<flxDevADS1015, &flxDevADS1015::get_sample_rate, &flxDevADS1015::set_sample_rate> sampleRate
     = { ADS1015_CONFIG_RATE_1600HZ, { { "128 Hz", ADS1015_CONFIG_RATE_128HZ }, { "250 Hz", ADS1015_CONFIG_RATE_250HZ },
                                       { "490 Hz", ADS1015_CONFIG_RATE_490HZ }, { "920 Hz", ADS1015_CONFIG_RATE_920HZ },
                                       { "1600 Hz", ADS1015_CONFIG_RATE_1600HZ }, { "2400 Hz", ADS1015_CONFIG_RATE_2400HZ },
                                       { "3300 Hz", ADS1015_CONFIG_RATE_3300HZ } } };
    flxPropertyRWUint16<flxDevADS1015, &flxDevADS1015::get_pga_gain, &flxDevADS1015::set_pga_gain> gain
     = { ADS1015_CONFIG_PGA_2, { { "x2/3", ADS1015_CONFIG_PGA_TWOTHIRDS }, { "x1", ADS1015_CONFIG_PGA_1 },
                                 { "x2", ADS1015_CONFIG_PGA_2 }, { "x4", ADS1015_CONFIG_PGA_4 },
                                 { "x8", ADS1015_CONFIG_PGA_8 }, { "x16", ADS1015_CONFIG_PGA_16 } } };

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_single_0> channel0;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_single_1> channel1;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_single_2> channel2;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_single_3> channel3;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_differential_P0_N1> differential_0_minus_1;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_differential_P0_N3> differential_0_minus_3;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_differential_P1_N3> differential_1_minus_3;
    flxParameterOutFloat<flxDevADS1015, &flxDevADS1015::read_differential_P2_N3> differential_2_minus_3;
};
