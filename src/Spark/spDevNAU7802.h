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

#include "spDevice.h"
#include "SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h"

// What is the name used to ID this device?
#define kNAU7802DeviceName "nau7802"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevNAU7802 : public spDeviceType<spDevNAU7802>, public NAU7802
{

public:
    spDevNAU7802();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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
    uint32_t read_zero_offset();
    float read_calibration_factor();
    float read_weight();
    // methods used to set our input parameters
    void calculate_zero_offset(const bool &dummy);
    void calculate_calibration_factor(const float &weight_in_units);

public:
    spPropertyUint<spDevNAU7802> zeroOffset;
    spPropertyFloat<spDevNAU7802> calibrationFactor;

    // Define our input parameters - specify the get functions to call.
    spParameterInBool<spDevNAU7802, &spDevNAU7802::calculate_zero_offset> calculateZeroOffset;
    spParameterInFloat<spDevNAU7802, &spDevNAU7802::calculate_calibration_factor> calculateCalibrationFactor;

    // Define our output parameters - specify the get functions to call.
    spParameterOutUint<spDevNAU7802, &spDevNAU7802::read_zero_offset> zeroOffset_output;
    spParameterOutFloat<spDevNAU7802, &spDevNAU7802::read_calibration_factor> calibrationFactor_output;
    spParameterOutFloat<spDevNAU7802, &spDevNAU7802::read_weight> weight;
};
