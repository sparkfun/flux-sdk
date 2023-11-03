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

#include "SparkFunOPT4048.h"
#include "flxCoreParam.h"
#include "flxCoreProps.h"
#include "flxDevice.h"
#include <cstdint>

// What is the name used to ID this device?
#define kOPT4048DeviceName "QwOpt4048"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevOPT4048 : public flxDeviceI2CType<flxDevOPT4048>, public QwOpt4048
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

    bool set_range(uint8_t); 
    uint8_t get_range();
    bool set_conversion_time(uint8_t); 
    uint8_t get_conversion_time();
    bool set_operation_mode(uint8_t);
    uint8_t get_operation_mode();

  public:
    // Define our output parameters - specify the get functions to call.
    flxPropertyRWUint<flxDevOPT4048,  &flxDevOPT4048::get_range, &flxDevOPT4048::set_range> range;
    flxPropertyRWUint<flxDevOPT4048,  &flxDevOPT4048::get_conversion_time, &flxDevOPT4048::set_conversion_time> time;
    flxPropertyRWUint<flxDevOPT4048,  &flxDevOPT4048::get_operation_mode, &flxDevOPT4048::set_operation_mode> mode;
    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CIEx> CIEx;
    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CIEy> CIEy;
    flxParameterOutDouble<flxDevOPT4048, &flxDevOPT4048::get_CCT> CCT;
    flxParameterOutUint<flxDevOPT4048, &flxDevOPT4048::get_lux> Lux;
};
