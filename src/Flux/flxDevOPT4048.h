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
 *  flxDevOPT4048.h
 *
 *  Device object for the OPT4048 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_OPT4048.h"
#include "flxCoreParam.h"
#include "flxCoreProps.h"
#include "flxDevice.h"
#include <cstdint>

// What is the name used to ID this device?
#define kOPT4048DeviceName "OPT4048"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevOPT4048 : public flxDeviceI2CType<flxDevOPT4048>, public SparkFun_OPT4048
{

  public:
    flxDevOPT4048();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kOPT4048DeviceName;
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
    // methods used to get values for our output parameters
    double get_CIEx();
    double get_CIEy();
    double get_CCT();
    uint32_t get_lux();

    void set_range(uint);
    uint get_range();
    void set_conversion_time(uint);
    uint get_conversion_time();
    void set_operation_mode(uint);
    uint get_operation_mode();

    // cache our pre-startup properties

    uint _cacheRange;
    uint _cacheTime;
    uint _cacheMode;

  public:
    // Define our output parameters - specify the get functions to call.
    flxPropertyRWUint<flxDevOPT4048, &flxDevOPT4048::get_range, &flxDevOPT4048::set_range> range = {
        RANGE_2KLUX2,
        {{"2254 Lux", RANGE_2KLUX2},
         {"4509 Lux", RANGE_4KLUX5},
         {"9018 Lux", RANGE_9LUX},
         {"18036 Lux", RANGE_18LUX},
         {"36071 Lux", RANGE_36LUX},
         {"72142 Lux", RANGE_72LUX},
         {"144284 Lux", RANGE_144LUX},
         {"Auto Range", RANGE_AUTO}}};

    flxPropertyRWUint<flxDevOPT4048, &flxDevOPT4048::get_conversion_time, &flxDevOPT4048::set_conversion_time> time = {
        CONVERSION_TIME_1MS,
        {{"600us", CONVERSION_TIME_600US},
         {"1ms", CONVERSION_TIME_1MS},
         {"1.8ms", CONVERSION_TIME_1MS8},
         {"3.4ms", CONVERSION_TIME_3MS4},
         {"6.5ms", CONVERSION_TIME_6MS5},
         {"12.7ms", CONVERSION_TIME_12MS7},
         {"25ms", CONVERSION_TIME_25MS},
         {"50ms", CONVERSION_TIME_50MS},
         {"100ms", CONVERSION_TIME_100MS},
         {"200ms", CONVERSION_TIME_200MS},
         {"400ms", CONVERSION_TIME_400MS},
         {"800ms", CONVERSION_TIME_800MS}}};

    flxPropertyRWUint<flxDevOPT4048, &flxDevOPT4048::get_operation_mode, &flxDevOPT4048::set_operation_mode> mode = {
        OPERATION_MODE_POWER_DOWN,
        {{"Power Down", OPERATION_MODE_POWER_DOWN},
         {"Auto One Shot", OPERATION_MODE_AUTO_ONE_SHOT},
         {"One Shot", OPERATION_MODE_ONE_SHOT},
         {"Continuous", OPERATION_MODE_CONTINUOUS}}};

    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CIEx> CIEx;
    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CIEy> CIEy;
    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CCT> CCT;
    flxParameterOutUint<flxDevOPT4048, &flxDevOPT4048::get_lux> Lux;
};
