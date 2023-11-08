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
    // Implement the device interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kENS160DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects.
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

    // methods to set a parameter to use for temp compensation
    void setTemperatureCompParameter(flxParameterOutScalar &);

    // Relative humidity comp
    void setHumidityCompParameter(flxParameterOutScalar &);

    // This device does runtime checks - so add loop method
    bool loop(void);

  private:
    // Parameter methods
    uint8_t read_AQI();
    uint16_t read_TVOC();
    uint16_t read_ETOH();
    uint16_t read_ECO2();
    float read_TempC();
    float read_RH();

    // Property methods

    // operating mode.
    uint8_t get_operating_mode(void);
    void set_operating_mode(uint8_t);

    // Method to manually set the temp compensation -- the above read value is used as getter
    void set_temp_comp(float);

    // method to manually set the humidity compensation value - above read method is used as getter
    void set_humid_comp(float);

    // Instance data

    // Operating mode -- used for startup  caching
    uint8_t _opMode;

    // this device can have inputs for temp and humidity compensation -- these
    // variable store those values if set -- nullptr by default
    flxParameterOutScalar *_tempCComp;
    flxParameterOutScalar *_rhComp;

    // last time the compensation values were set -- when a input device is provided
    uint32_t _lastCompCheck;

  public:
    // properties
    // Operating mode prop
    flxPropertyRWUint8<flxDevENS160, &flxDevENS160::get_operating_mode, &flxDevENS160::set_operating_mode>
        operatingMode = {
            SFE_ENS160_STANDARD,
            {{"Standard", SFE_ENS160_STANDARD}, {"Idle", SFE_ENS160_IDLE}, {"Deep Sleep", SFE_ENS160_DEEP_SLEEP}}};

    // Compensation settings -- enabled by default - will start operating if a compensation source is added
    flxPropertyBool<flxDevENS160> enableCompensation = true;

    // If a device is connected for comp values, update period in secs
    flxPropertyUint<flxDevENS160> updatePeriodSecs = {kENS160DefaultCompUpdateTimeSecs, 5, 600};

    // properties to manually set / get the compensation values
    flxPropertyRWFloat<flxDevENS160, &flxDevENS160::read_TempC, &flxDevENS160::set_temp_comp> tempComp;
    flxPropertyRWFloat<flxDevENS160, &flxDevENS160::read_RH, &flxDevENS160::set_humid_comp> humidityComp;

    // Define our output parameters - specify the get functions to call.

    flxParameterOutUint8<flxDevENS160, &flxDevENS160::read_AQI> val_AQI;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_TVOC> val_TVOC;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_ETOH> val_ETOH;
    flxParameterOutUint16<flxDevENS160, &flxDevENS160::read_ECO2> val_ECO2;
    flxParameterOutFloat<flxDevENS160, &flxDevENS160::read_TempC> val_TempC;
    flxParameterOutFloat<flxDevENS160, &flxDevENS160::read_RH> val_RH;
};
