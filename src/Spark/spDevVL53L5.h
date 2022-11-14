/*
 *
 *  spDevVL53L5.h
 *
 *  Spark Device object for the VL53L5 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_VL53L5CX_Library.h"

// What is the name used to ID this device?
#define kVL53L5DeviceName "VL53L5"

//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevVL53L5 : public spDeviceI2CType<spDevVL53L5>, public SparkFun_VL53L5CX
{

public:
    spDevVL53L5();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spBusI2C &i2cDriver, uint8_t address);
    
    static const char *getDeviceName()
    {
        return kVL53L5DeviceName;
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
    bool read_distance(spDataArrayInt16 *);

    // methods to get/set our read-write properties
    uint32_t get_integration_time();
    void set_integration_time(uint32_t);
    uint8_t get_sharpener_percent();
    void set_sharpener_percent(uint8_t);
    uint8_t get_target_order();
    void set_target_order(uint8_t);

    uint32_t _integrationTime = 5; // Default is 5ms
    uint8_t _sharpenerPercent = 5; // Default is 5%
    uint8_t _targetOrder = (uint8_t)SF_VL53L5CX_TARGET_ORDER::STRONGEST;

    bool _begun = false;

public:
    // Define our read-write properties
    spPropertyRWUint<spDevVL53L5, &spDevVL53L5::get_integration_time, &spDevVL53L5::set_integration_time> integrationTime
         = { 5, 2, 1000 };
    spPropertyRWUint8<spDevVL53L5, &spDevVL53L5::get_sharpener_percent, &spDevVL53L5::set_sharpener_percent> sharpenerPercent
        = { 5, 0, 99 };
    spPropertyRWUint8<spDevVL53L5, &spDevVL53L5::get_target_order, &spDevVL53L5::set_target_order> targetOrder
        = { (uint8_t)SF_VL53L5CX_TARGET_ORDER::STRONGEST, { { "Strongest", (uint8_t)SF_VL53L5CX_TARGET_ORDER::STRONGEST}, { "Closest", (uint8_t)SF_VL53L5CX_TARGET_ORDER::CLOSEST} } };

    // Define our output parameters - specify the get functions to call.
    spParameterOutArrayInt16<spDevVL53L5, &spDevVL53L5::read_distance> distance;    
};
