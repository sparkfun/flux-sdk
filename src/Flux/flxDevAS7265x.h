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
 *  flxDevAS7265X.h
 *
 *  Spark Device object for the AS7265X Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_AS7265X.h"
#include "flxDevice.h"

#define kAS7265xDeviceName "AS7265X"

// Define our class
class flxDevAS7265X : public flxDeviceI2CType<flxDevAS7265X>, public AS7265X
{

  public:
    flxDevAS7265X();
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

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

    bool get_indicator(void);
    void set_indicator(bool);

    bool _indicator;

    uint8_t get_gain(void);
    void set_gain(uint8_t);

    uint8_t _gain;

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

    flxPropertyRWUint8<flxDevAS7265X, &flxDevAS7265X::get_gain, &flxDevAS7265X::set_gain> sensorGain = {
        AS7265X_GAIN_16X,
        {{"1X", AS7265X_GAIN_1X}, {"3.7X", AS7265X_GAIN_37X}, {"16X", AS7265X_GAIN_16X}, {"64X", AS7265X_GAIN_64X}}};

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
