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
 *  flxDevFS3000.h
 *
 *  Spark Device object for the FS3000 device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_FS3000_Arduino_Library.h"
#include "flxDevice.h"



#define kFS3000DeviceName "FS3000"

// Define our class
class flxDevFS3000 : public flxDeviceI2CType<flxDevFS3000>, public FS3000
{

public:
    flxDevFS3000();
    // Interface
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kFS3000DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_mps();
    float read_mph();

    bool _fs3000_1005 = true;

    uint8_t get_fs3000_version();
    void set_fs3000_version(uint8_t);

public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevFS3000, &flxDevFS3000::read_mps> flow_mps;
    flxParameterOutFloat<flxDevFS3000, &flxDevFS3000::read_mph> flow_mph;

    flxPropertyRWUint8<flxDevFS3000, &flxDevFS3000::get_fs3000_version, &flxDevFS3000::set_fs3000_version> fs3000version
         = { 1 , { { "FS3000_1005", 1 }, { "FS3000_1015", 0 } } };
};
