/*
 *
 *  spDevVL53L1X.h
 *
 *  Spark Device object for the VL53L1X device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_VL53L1X.h"

// What is the name used to ID this device?
#define kVL53L1XDeviceName "vl53l1x"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevVL53L1X : public spDeviceType<spDevVL53L1X>, public SFEVL53L1X
{

public:
    spDevVL53L1X();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    uint read_distance();
    uint read_range_status();
    uint read_signal_rate();

    // methods to get/set our read-write properties
    bool get_distance_mode();
    void set_distance_mode(bool);
    uint get_intermeasurment_period();
    void set_intermeasurment_period(uint);
    uint get_crosstalk();
    void set_crosstalk(uint);
    uint get_offset();
    void set_offset(uint);

    bool _shortDistanceMode = true; // Default to short distance mode

public:
    // Define our read-write properties
    spPropertyRWBool<spDevVL53L1X, &spDevVL53L1X::get_distance_mode, &spDevVL53L1X::set_distance_mode> distanceMode;
    spPropertyRWUint<spDevVL53L1X, &spDevVL53L1X::get_intermeasurment_period, &spDevVL53L1X::set_intermeasurment_period> intermeasurementPeriod;
    spPropertyRWUint<spDevVL53L1X, &spDevVL53L1X::get_crosstalk, &spDevVL53L1X::set_crosstalk> crosstalk;
    spPropertyRWUint<spDevVL53L1X, &spDevVL53L1X::get_offset, &spDevVL53L1X::set_offset> offset;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevVL53L1X, &spDevVL53L1X::read_distance> distance;    
    spParameterOutUint<spDevVL53L1X, &spDevVL53L1X::read_range_status> rangeStatus;    
    spParameterOutUint<spDevVL53L1X, &spDevVL53L1X::read_signal_rate> signalRate;    
};
