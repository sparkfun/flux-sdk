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
 *  flxDevVL53L1X.h
 *
 *  Spark Device object for the VL53L1X device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_VL53L1X.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kVL53L1XDeviceName "VL53L1x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevVL53L1X : public flxDeviceI2CType<flxDevVL53L1X>, public SFEVL53L1X
{

  public:
    flxDevVL53L1X();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kVL53L1XDeviceName;
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
    uint32_t read_distance();
    uint32_t read_range_status();
    uint32_t read_signal_rate();

    // methods to get/set our read-write properties
    uint8_t get_distance_mode();
    void set_distance_mode(uint8_t);
    uint16_t get_intermeasurment_period();
    void set_intermeasurment_period(uint16_t);
    uint16_t get_crosstalk();
    void set_crosstalk(uint16_t);
    uint16_t get_offset();
    void set_offset(uint16_t);

    bool _shortDistanceMode = true; // Default to short distance mode
    uint16_t _intermeasurementPeriod = 100;
    uint16_t _crosstalk = 0;
    uint16_t _offset = 0;

  public:
    // Define our read-write properties
    flxPropertyRWUInt8<flxDevVL53L1X, &flxDevVL53L1X::get_distance_mode, &flxDevVL53L1X::set_distance_mode>
        distanceMode = {DISTANCE_SHORT,
                        {{"Short", DISTANCE_SHORT}, {"Long", DISTANCE_LONG}}}; // Default to short distance mode
    flxPropertyRWUInt16<flxDevVL53L1X, &flxDevVL53L1X::get_intermeasurment_period,
                        &flxDevVL53L1X::set_intermeasurment_period>
        intermeasurementPeriod;
    flxPropertyRWUInt16<flxDevVL53L1X, &flxDevVL53L1X::get_crosstalk, &flxDevVL53L1X::set_crosstalk> crosstalk = {0, 0,
                                                                                                                  4000};
    flxPropertyRWUInt16<flxDevVL53L1X, &flxDevVL53L1X::get_offset, &flxDevVL53L1X::set_offset> offset = {0, 0, 4000};

    // Define our output parameters - specify the get functions to call.
    flxParameterOutUInt32<flxDevVL53L1X, &flxDevVL53L1X::read_distance> distance;
    flxParameterOutUInt32<flxDevVL53L1X, &flxDevVL53L1X::read_range_status> rangeStatus;
    flxParameterOutUInt32<flxDevVL53L1X, &flxDevVL53L1X::read_signal_rate> signalRate;
};
