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
 *  flxDevADS122C04.h
 *
 *  Spark Device object for the ADS122C04 A/D converter device.
 *
 *
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_ADS122C04_ADC_Arduino_Library.h"
#include "flxDevice.h"

#define kADS122C04DeviceName "ADS122C04"

// Define our class
class flxDevADS122C04 : public flxDeviceI2CType<flxDevADS122C04>, public SFE_ADS122C04
{

  public:
    flxDevADS122C04();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kADS122C04DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

  private:
    float read_temperature_c();
    float read_temperature_f();
    float read_internal_temperature();
    float read_raw_voltage();

    uint8_t get_wire_mode();
    void set_wire_mode(uint8_t);
    uint8_t get_sample_rate();
    void set_sample_rate(uint8_t);

    uint8_t _wireMode;
    uint8_t _sampleRate;

  public:
    flxPropertyRWUInt8<flxDevADS122C04, &flxDevADS122C04::get_wire_mode, &flxDevADS122C04::set_wire_mode> wireMode = {
        ADS122C04_4WIRE_MODE,
        {{"4-Wire Mode", ADS122C04_4WIRE_MODE},
         {"3-Wire Mode", ADS122C04_3WIRE_MODE},
         {"2-Wire Mode", ADS122C04_2WIRE_MODE},
         {"Internal Temperature Mode", ADS122C04_TEMPERATURE_MODE},
         {"Raw ADC Voltage Mode", ADS122C04_RAW_MODE},
         {"4-Wire Mode High Temperature", ADS122C04_4WIRE_HI_TEMP},
         {"3-Wire Mode High Temperature", ADS122C04_3WIRE_HI_TEMP},
         {"2-Wire Mode High Temperature", ADS122C04_2WIRE_HI_TEMP}}};
    flxPropertyRWUInt8<flxDevADS122C04, &flxDevADS122C04::get_sample_rate, &flxDevADS122C04::set_sample_rate>
        sampleRate = {ADS122C04_DATA_RATE_20SPS,
                      {{"20 Samples Per Sec", ADS122C04_DATA_RATE_20SPS},
                       {"45 Samples Per Sec", ADS122C04_DATA_RATE_45SPS},
                       {"90 Samples Per Sec", ADS122C04_DATA_RATE_90SPS},
                       {"175 Samples Per Sec", ADS122C04_DATA_RATE_175SPS},
                       {"330 Samples Per Sec", ADS122C04_DATA_RATE_330SPS},
                       {"600 Samples Per Sec", ADS122C04_DATA_RATE_600SPS},
                       {"1000 Samples Per Sec", ADS122C04_DATA_RATE_1000SPS}}};

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevADS122C04, &flxDevADS122C04::read_temperature_c> temperatureC;
    flxParameterOutFloat<flxDevADS122C04, &flxDevADS122C04::read_temperature_f> temperatureF;
    flxParameterOutFloat<flxDevADS122C04, &flxDevADS122C04::read_internal_temperature> internalTemperature;
    flxParameterOutFloat<flxDevADS122C04, &flxDevADS122C04::read_raw_voltage> rawVoltage;
};
