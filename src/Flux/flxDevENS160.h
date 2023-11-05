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
 *  flxDevENS160.h
 *
 *  Spark Device object for the ENS160 Qwiic device.
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_ENS160.h"
#include "flxDevice.h"

#define kENS160DeviceName "ENS160"

#define kENS160DefaultCompUpdateTimeSecs 60

// Define our class
class flxDevENS160 : public flxDeviceI2CType<flxDevENS160>, public SparkFun_ENS160
{

  public:
    flxDevENS160();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kENS160DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);


    // methods to set a parameter to use for temp compensation
    void setTemperatureCompParameter(flxParameterOutScalar&);

    // Relitive humidity comp
    void setHumidityCompParameter(flxParameterOutScalar&);    

    bool loop(void);


  private:
    uint8_t read_AQI();
    uint16_t read_TVOC();
    uint16_t read_ETOH();
    uint16_t read_ECO2();
    float read_TempC();
    float read_RH();

    uint8_t get_operating_mode(void);
    void set_operating_mode(uint8_t);

    uint8_t _opMode;

    flxParameterOutScalar *  _tempCComp; 
    flxParameterOutScalar *  _rhComp;

    uint32_t  _lastCompCheck;

  public:
    // properties
    flxPropertyRWUint8<flxDevENS160, &flxDevENS160::get_operating_mode, &flxDevENS160::set_operating_mode> operatingMode = {
        SFE_ENS160_STANDARD,
        {{"Standard", SFE_ENS160_STANDARD}, {"Idle", SFE_ENS160_IDLE}, {"Deep Sleep", SFE_ENS160_DEEP_SLEEP}}};

    // Compensation settings
    flxPropertyBool<flxDevENS160> enableCompensation = false;
    flxPropertyUint<flxDevENS160> updatePeriodSecs = {kENS160DefaultCompUpdateTimeSecs, 5, 600};

    // Define our output parameters - specify the get functions to call.

    flxParameterOutUint8<flxDevENS160, &flxDevENS160::read_AQI> val_AQI;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_TVOC> val_TVOC;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_ETOH> val_ETOH;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_ECO2> val_ECO2;
    flxParameterOutFloat<flxDevENS160, &flxDevENS160::read_TempC> val_TempC;
    flxParameterOutFloat<flxDevENS160, &flxDevENS160::read_RH> val_RH;
};
