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
 * QwiicDevNAU7802.h
 *
 *  Device object for the NAU7802 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"
#include "flxDevice.h"

// What is the name used to ID this device?
#define kNAU7802DeviceName "NAU7802"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class flxDevNAU7802 : public flxDeviceI2CType<flxDevNAU7802>, public NAU7802
{

  public:
    flxDevNAU7802();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);

    static flxDeviceConfidence_t connectedConfidence(void)
    {
        return flxDevConfidenceExact;
    }

    static const char *getDeviceName()
    {
        return kNAU7802DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    // Method called to initialize the class
    bool onInitialize(TwoWire &);

    // Called when a managed property is updated
    void onPropertyUpdate(const char *);

  private:
    // methods used to get values for our output parameters
    float read_weight();

    // methods used to get values for our RW properties
    int32_t get_zero_offset();
    void set_zero_offset(int32_t);
    float get_calibration_factor();

    void set_calibration_factor(float);

    // methods for the external calibration offset and gain hidden properties
    int32_t get_ext_offset(void);
    void set_ext_offset(int32_t);

    uint32_t get_ext_gain(void);
    void set_ext_gain(uint32_t);

    // methods used to set our input parameters
    void calculate_zero_offset();
    void calculate_calibration_factor(const float &weight_in_units);

  public:
    flxPropertyRWInt32<flxDevNAU7802, &flxDevNAU7802::get_zero_offset, &flxDevNAU7802::set_zero_offset> zeroOffset;
    flxPropertyRWFloat<flxDevNAU7802, &flxDevNAU7802::get_calibration_factor, &flxDevNAU7802::set_calibration_factor>
        calibrationFactor;

    // External calibration - hidden properties for cal data.
    flxPropertyRWHiddenInt32<flxDevNAU7802, &flxDevNAU7802::get_ext_offset, &flxDevNAU7802::set_ext_offset>
        _externalCalOffset;
    flxPropertyRWHiddenUInt32<flxDevNAU7802, &flxDevNAU7802::get_ext_gain, &flxDevNAU7802::set_ext_gain>
        _externalCalGain;

    // Define our input parameters - specify the get functions to call.
    flxParameterInVoid<flxDevNAU7802, &flxDevNAU7802::calculate_zero_offset> calculateZeroOffset;
    flxParameterInFloat<flxDevNAU7802, &flxDevNAU7802::calculate_calibration_factor> calculateCalibrationFactor;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevNAU7802, &flxDevNAU7802::read_weight> weight;
};
