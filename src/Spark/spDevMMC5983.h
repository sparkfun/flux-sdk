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

#include "spDevice.h"
#include "SparkFun_MMC5983MA_Arduino_Library.h"

// What is the name used to ID this device?
#define kMMC5983DeviceName "mmc5983"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevMMC5983 : public spDeviceType<spDevMMC5983>, public SFE_MMC5983MA
{

public:
    spDevMMC5983();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
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

    // Called when a managed property is updated
    void onPropertyUpdate(const char *);

private:
    // methods used to get values for our output parameters
    double read_x();
    double read_y();
    double read_z();
    int read_temperature();

    // methods used to get values for our RW properties
    uint16_t get_filter_bandwidth();
    void set_filter_bandwidth(uint16_t);
    uint16_t get_continuous_frequency();
    void set_continuous_frequency(uint16_t);
    uint8_t get_auto_reset();
    void set_auto_reset(uint8_t);

    uint16_t _filter_bandwidth = 100;
    uint16_t _continuous_frequency = 100;
    bool _auto_reset = true;

    bool _begun = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutDouble<spDevMMC5983, &spDevMMC5983::read_x> magX;
    spParameterOutDouble<spDevMMC5983, &spDevMMC5983::read_y> magY;
    spParameterOutDouble<spDevMMC5983, &spDevMMC5983::read_z> magZ;
    spParameterOutInt<spDevMMC5983, &spDevMMC5983::read_temperature> temperature;

    // Define our read-write properties
    spPropertyRWUint16<spDevMMC5983, &spDevMMC5983::get_filter_bandwidth, &spDevMMC5983::set_filter_bandwidth> filterBandwidth
        = { 100, { {"100 Hz", 100}, {"200 Hz", 200}, {"400 Hz", 400}, {"800 Hz", 800} } };
    spPropertyRWUint16<spDevMMC5983, &spDevMMC5983::get_continuous_frequency, &spDevMMC5983::set_continuous_frequency> continuousFrequency
        = { 100, { {"Off", 0}, {"1 Hz", 1}, {"10 Hz", 10}, {"20 Hz", 20}, {"50 Hz", 50}, {"100 Hz", 100}, {"200 Hz", 200}, {"1000 Hz", 1000} } };
    spPropertyRWUint8<spDevMMC5983, &spDevMMC5983::get_auto_reset, &spDevMMC5983::set_auto_reset> autoReset
        = { 1, { {"Enabled", 1}, {"Disabled", 0} } };
};
