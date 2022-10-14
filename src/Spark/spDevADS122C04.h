/*
 *
 *  spDevADS122C04.h
 *
 *  Spark Device object for the ADS122C04 A/D converter device.
 * 
 * 
 */

#pragma once

#include "Arduino.h"
#include "SparkFun_ADS122C04_ADC_Arduino_Library.h"
#include "spDevice.h"



#define kADS122C04DeviceName "ads122c04"

// Define our class
class spDevADS122C04 : public spDeviceType<spDevADS122C04>, public SFE_ADS122C04
{

  public:
    spDevADS122C04();
    // Interface
    static bool isConnected(spDevI2C &i2cDriver, uint8_t address);

    static const char *getDeviceName()
    {
        return kADS122C04DeviceName;
    };

    static const uint8_t *getDefaultAddresses()
    {
        return defaultDeviceAddress;
    }
    // holds the class list of possible addresses/IDs for this objects
    static uint8_t defaultDeviceAddress[];

    bool onInitialize(TwoWire &);

private:
    float read_temperature_c();
    float read_temperature_f();
    float read_internal_temperature();
    float read_raw_voltage();

    uint8_t get_wire_mode();
    void set_wire_mode(uint8_t);
    uint8_t get_sample_rate();
    void set_sample_rate(uint8_t);

    uint8_t _wireMode;
    uint8_t _sampleRate;

public:
    spPropertyRWUint8<spDevADS122C04, &spDevADS122C04::get_wire_mode, &spDevADS122C04::set_wire_mode> wireMode;
    spPropertyRWUint8<spDevADS122C04, &spDevADS122C04::get_sample_rate, &spDevADS122C04::set_sample_rate> sampleRate;

    // Define our output parameters - specify the get functions to call.
    spParameterOutFloat<spDevADS122C04, &spDevADS122C04::read_temperature_c> temperatureC;
    spParameterOutFloat<spDevADS122C04, &spDevADS122C04::read_temperature_f> temperatureF;    
    spParameterOutFloat<spDevADS122C04, &spDevADS122C04::read_internal_temperature> internalTemperature;    
    spParameterOutFloat<spDevADS122C04, &spDevADS122C04::read_raw_voltage> rawVoltage;    

};
