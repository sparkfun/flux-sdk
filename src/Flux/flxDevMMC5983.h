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
 * QwiicDevMMC5983.h
 *
 *  Device object for the MMC5983 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_MMC5983MA_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kMMC5983DeviceName "MMC5983"
//----------------------------------------------------------------------------------------------------------
// Define our base class - I2C and SPI version sub-class from this class
class flxDevMMC5983Base : public flxDevice, public SFE_MMC5983MA
{

  public:
    flxDevMMC5983Base();

  private:
    // methods used to get values for our output parameters
    double read_x();
    double read_y();
    double read_z();
    int read_temperature();

    // methods used to get values for our RW properties
    uint16_t get_filter_bandwidth();
    void set_filter_bandwidth(uint16_t);
    uint8_t get_auto_reset();
    void set_auto_reset(uint8_t);

    uint16_t _filter_bandwidth = 100;
    bool _auto_reset = true;

    // Flags to prevent getMeasurementXYZ being called mulitple times
    bool _magX = false;
    bool _magY = false;
    bool _magZ = false;

    uint32_t _rawX = 0;
    uint32_t _rawY = 0;
    uint32_t _rawZ = 0;

  protected:
    bool onInitialize(void);

  public:
    // Define our output parameters - specify the get functions to call.
    flxParameterOutDouble<flxDevMMC5983Base, &flxDevMMC5983Base::read_x> magX;
    flxParameterOutDouble<flxDevMMC5983Base, &flxDevMMC5983Base::read_y> magY;
    flxParameterOutDouble<flxDevMMC5983Base, &flxDevMMC5983Base::read_z> magZ;
    flxParameterOutInt<flxDevMMC5983Base, &flxDevMMC5983Base::read_temperature> temperature;

    // Define our read-write properties
    flxPropertyRWUint16<flxDevMMC5983Base, &flxDevMMC5983Base::get_filter_bandwidth,
                       &flxDevMMC5983Base::set_filter_bandwidth>
        filterBandwidth = {100, {{"100 Hz", 100}, {"200 Hz", 200}, {"400 Hz", 400}, {"800 Hz", 800}}};
    flxPropertyRWUint8<flxDevMMC5983Base, &flxDevMMC5983Base::get_auto_reset, &flxDevMMC5983Base::set_auto_reset>
        autoReset = {1, {{"Enabled", 1}, {"Disabled", 0}}};
};

//----------------------------------------------------------------------------------------------------------
// I2C class definition
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevMMC5983 : public flxDeviceI2CType<flxDevMMC5983, flxDevMMC5983Base>
{

  public:
    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }
    
    static const char *getDeviceName()
    {
        return kMMC5983DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);
};

//----------------------------------------------------------------------------------------------------------
// SPI class definition
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevMMC5983_SPI : public flxDeviceSPIType<flxDevMMC5983_SPI, flxDevMMC5983Base>
{

  public:
    // Method called to initialize the class
    bool onInitialize(SPIClass &);
};