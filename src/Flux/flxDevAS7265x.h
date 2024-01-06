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
 *  flxDevAS7265X.h
 *
 *  Spark Device object for the AS7265X Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_AS7265X.h"
#include "flxDevice.h"

#define kAS7265xDeviceName "AS7265X"

#define kAS7265xDefaultIntCycles 49

// Define our class
class flxDevAS7265X : public flxDeviceI2CType<flxDevAS7265X>, public AS7265X
{

  public:
    flxDevAS7265X();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidencePing;
    }

    static const char *getDeviceName()
    {
        return kAS7265xDeviceName;
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
    // property methods

    // Indicator pro getter/setter/stash
    bool get_indicator(void);
    void set_indicator(bool);
    bool _indicator;

    // gain pro getter/setter/stash
    uint8_t get_gain(void);
    void set_gain(uint8_t);
    uint8_t _gain;

    // Measurement Mode pro getter/setter/stash
    uint8_t get_mmode(void);
    void set_mmode(uint8_t);
    uint8_t _mmode;

    // Integration Cycles pro getter/setter/stash
    uint8_t get_icycles(void);
    void set_icycles(uint8_t);
    uint8_t _icycles;

    // Indicator current pro getter/setter/stash
    uint8_t get_ind_current(void);
    void set_ind_current(uint8_t);
    uint8_t _ind_current;

    // White LED current pro getter/setter/stash
    uint8_t get_white_current(void);
    void set_white_current(uint8_t);
    uint8_t _white_current;

    // IR LED current pro getter/setter/stash
    uint8_t get_ir_current(void);
    void set_ir_current(uint8_t);
    uint8_t _ir_current;

    // Parameter methods
    bool read_output_type(void);
    float read_A(void);
    float read_B(void);
    float read_C(void);
    float read_D(void);
    float read_E(void);
    float read_F(void);

    float read_G(void);
    float read_H(void);
    float read_I(void);
    float read_J(void);
    float read_K(void);
    float read_L(void);

    float read_R(void);
    float read_S(void);
    float read_T(void);
    float read_U(void);
    float read_V(void);
    float read_W(void);

  public:
    // Properties

    // Read with LED active?

    flxPropertyBool<flxDevAS7265X> readWithLED = {false};

    flxPropertyRWBool<flxDevAS7265X, &flxDevAS7265X::get_indicator, &flxDevAS7265X::set_indicator> enableIndicator = {
        true};

    flxPropertyBool<flxDevAS7265X> outputCal = {true};

    // Gain prop
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_gain, &flxDevAS7265X::set_gain> sensorGain = {
        AS7265X_GAIN_16X,
        {{"1X", AS7265X_GAIN_1X}, {"3.7X", AS7265X_GAIN_37X}, {"16X", AS7265X_GAIN_16X}, {"64X", AS7265X_GAIN_64X}}};

    // Measurement mode prop
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_mmode, &flxDevAS7265X::set_mmode> measureMode = {
        AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT,
        {{"STUV Channels", AS7265X_MEASUREMENT_MODE_4CHAN},
         {"RTUW Channels", AS7265X_MEASUREMENT_MODE_4CHAN_2},
         {"6 Channel Continuous", AS7265X_MEASUREMENT_MODE_6CHAN_CONTINUOUS},
         {"6 Channel 1 Shot", AS7265X_MEASUREMENT_MODE_6CHAN_ONE_SHOT}}};

    // Integration Cycles
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_icycles, &flxDevAS7265X::set_icycles> intCycles = {
        kAS7265xDefaultIntCycles, 0, 255};

    // Indicator Current prop
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_ind_current, &flxDevAS7265X::set_ind_current>
        indicatorCurrent = {AS7265X_INDICATOR_CURRENT_LIMIT_8MA,
                            {{"1 ma", AS7265X_INDICATOR_CURRENT_LIMIT_1MA},
                             {"2 ma", AS7265X_INDICATOR_CURRENT_LIMIT_2MA},
                             {"4 ma", AS7265X_INDICATOR_CURRENT_LIMIT_4MA},
                             {"8 ma", AS7265X_INDICATOR_CURRENT_LIMIT_8MA}}};

    // White LED Current prop
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_white_current, &flxDevAS7265X::set_white_current>
        whiteCurrent = {AS7265X_LED_CURRENT_LIMIT_12_5MA,
                        {{"12.5 ma", AS7265X_LED_CURRENT_LIMIT_12_5MA},
                         {"25 ma", AS7265X_LED_CURRENT_LIMIT_25MA},
                         {"50 ma", AS7265X_LED_CURRENT_LIMIT_50MA},
                         {"100 ma", AS7265X_LED_CURRENT_LIMIT_100MA}}};

    // IR LED Current prop
    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_ir_current, &flxDevAS7265X::set_ir_current> irCurrent = {
        AS7265X_LED_CURRENT_LIMIT_12_5MA,
        {{"12.5 ma", AS7265X_LED_CURRENT_LIMIT_12_5MA},
         {"25 ma", AS7265X_LED_CURRENT_LIMIT_25MA},
         {"50 ma", AS7265X_LED_CURRENT_LIMIT_50MA}}};

    // Outputs
    // define parameters for calibrarted outputs
    flxParameterOutBool<flxDevAS7265X, &flxDevAS7265X::read_output_type> outputType;

    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_A> outA;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_B> outB;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_C> outC;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_D> outD;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_E> outE;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_F> outF;

    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_G> outG;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_H> outH;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_I> outI;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_J> outJ;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_K> outK;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_L> outL;

    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_R> outR;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_S> outS;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_T> outT;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_U> outU;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_V> outV;
    flxParameterOutFloat<flxDevAS7265X, &flxDevAS7265X::read_W> outW;
};
