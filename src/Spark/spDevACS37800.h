/*
 *
 * QwiicDevACS37800.h
 *
 *  Device object for the ACS37800 Qwiic device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "spDevice.h"
#include "SparkFun_ACS37800_Arduino_Library.h"

// What is the name used to ID this device?
#define kACS37800DeviceName "ACS37800"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Qwiic Library
class spDevACS37800 : public spDeviceI2CType<spDevACS37800>, public ACS37800
{

public:
    spDevACS37800();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kACS37800DeviceName;
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
    float read_volts();
    float read_amps();
    float read_watts();
    float read_volts_rms();
    float read_amps_rms();
    float read_power_active();
    float read_power_reactive();
    float read_power_apparent();
    float read_power_factor();
    bool read_pos_angle();
    bool read_pos_power_factor();

    // methods used to get values for our RW properties
    uint get_number_of_samples();
    void set_number_of_samples(uint);
    uint8_t get_bypass_n_enable();
    void set_bypass_n_enable(uint8_t);
    void set_sense_resistance(float);
    float get_sense_resistance();
    void set_divider_resistance(float);
    float get_divider_resistance();
    void set_current_range(float);
    float get_current_range();

    // Flags to prevent readInstantaneous being called multiple times
    bool _volts = false;
    bool _amps = false;
    bool _watts = false;
    float _theVolts = 0.;
    float _theAmps = 0.;
    float _theWatts = 0.;

    // Flags to prevent readRMS being called multiple times
    bool _volts_rms = false;
    bool _amps_rms = false;
    float _theVoltsRMS = 0.;
    float _theAmpsRMS = 0.;
    
    // Flags to prevent readPowerActiveReactive being called multiple times
    bool _active = false;
    bool _reactive = false;
    float _theActive = 0.;
    float _theReactive = 0.;
    
    // Flags to prevent readPowerFactor being called multiple times
    bool _apparent = false;
    bool _factor = false;
    bool _posAngle = false;
    bool _posPF = false;
    float _theApparent = 0.;
    float _theFactor = 0.;
    bool _thePosAngle =  false;
    bool _thePosPF = false;

    bool _bypassNenable = true; // For DC measurement, bypass_n should be enabled
    uint32_t _n = 1023; // Default to 1023 samples (at 32kHz, for DC measurement)

    float _senseResistance = ACS37800_DEFAULT_SENSE_RES;
    float _dividerResistance = ACS37800_DEFAULT_DIVIDER_RES;
    float _currentRange = ACS37800_DEFAULT_CURRENT_RANGE;

    bool _begun = false;

public:
    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_volts> volts;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_amps> amps;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_watts> watts;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_volts_rms> voltsRMS;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_amps> ampsRMS;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_power_active> powerActive;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_power_reactive> powerReactive;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_power_apparent> powerApparent;
    spParameterOutFloat<spDevACS37800, &spDevACS37800::read_power_factor> powerFactor;
    spParameterOutBool<spDevACS37800, &spDevACS37800::read_pos_angle> positiveAngle;
    spParameterOutBool<spDevACS37800, &spDevACS37800::read_pos_power_factor> positivePowerFactor;

    // Define our read-write properties
    spPropertyRWUint<spDevACS37800, &spDevACS37800::get_number_of_samples, &spDevACS37800::set_number_of_samples> numberOfSamples = { 0, 0, 1023 };
    spPropertyRWUint8<spDevACS37800, &spDevACS37800::get_bypass_n_enable, &spDevACS37800::set_bypass_n_enable> bypassNenable
        = { 0, { {"RMS calculated using zero crossings", 0}, {"RMS calculated using n samples", 1} } };
    spPropertyRWFloat<spDevACS37800, &spDevACS37800::get_sense_resistance, &spDevACS37800::set_sense_resistance> senseResistance;
    spPropertyRWFloat<spDevACS37800, &spDevACS37800::get_divider_resistance, &spDevACS37800::set_divider_resistance> dividerResistance;
    spPropertyRWFloat<spDevACS37800, &spDevACS37800::get_current_range, &spDevACS37800::set_current_range> currentRange;
};
