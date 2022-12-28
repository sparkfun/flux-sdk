/*
 *
 *  flxDevSEN54.h
 *
 *  Spark Device object for the SEN54 device.
 *
 *
 *
 */

#pragma once

#include "Arduino.h"

#include "flxDevice.h"
#include "SensirionI2CSen5x.h" // Also requires Sensirion Core

// What is the name used to ID this device?
#define kSEN54DeviceName "SEN54"
//----------------------------------------------------------------------------------------------------------
// Define our class - note we are sub-classing from the Sensirion Library
class flxDevSEN54 : public flxDeviceI2CType<flxDevSEN54>, public SensirionI2CSen5x
{

public:
    flxDevSEN54();

    // Static Interface - used by the system to determine if this device is
    // connected before the object is instantiated.
    static bool isConnected(flxBusI2C &i2cDriver, uint8_t address);
    static const char *getDeviceName()
    {
        return kSEN54DeviceName;
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

    // methods used to get values for our RW properties
    float get_temperature_offset();
    void set_temperature_offset(float);

    // methods used to get values for our output parameters
    float read_temperature_C();
    float read_humidity();
    float read_mass_concentration_1p0();
    float read_mass_concentration_2p5();
    float read_mass_concentration_4p0();
    float read_mass_concentration_10p0();
    float read_voc_index();
    float read_nox_index();

    // Flags to prevent readMeasuredValues being called multiple times
    bool _temperature = false;
    bool _humidity = false;
    bool _massConcentrationPm1p0 = false;
    bool _massConcentrationPm2p5 = false;
    bool _massConcentrationPm4p0 = false;
    bool _massConcentrationPm10p0 = false;
    bool _ambientHumidity = false;
    bool _ambientTemperature = false;
    bool _vocIndex = false;
    bool _noxIndex = false;

    float _theTemperature = -999.0;
    float _theHumidity = -999.0;
    float _theMassConcentrationPm1p0 = -999.0;
    float _theMassConcentrationPm2p5 = -999.0;
    float _theMassConcentrationPm4p0 = -999.0;
    float _theMassConcentrationPm10p0 = -999.0;
    float _theAmbientHumidity = -999.0;
    float _theAmbientTemperature = -999.0;
    float _theVocIndex = -999.0;
    float _theNoxIndex = -999.0;

    float _theTemperatureOffset = 0.0;

    bool _begun = false;

public:
    flxPropertyRWFloat<flxDevSEN54, &flxDevSEN54::get_temperature_offset, &flxDevSEN54::set_temperature_offset> temperatureOffset;

    // Define our output parameters - specify the get functions to call.
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_temperature_C> temperatureC;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_humidity> humidity;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_mass_concentration_1p0> massConcentrationPm1p0;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_mass_concentration_2p5> massConcentrationPm2p5;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_mass_concentration_4p0> massConcentrationPm4p0;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_mass_concentration_10p0> massConcentrationPm10p0;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_voc_index> vocIndex;    
    flxParameterOutFloat<flxDevSEN54, &flxDevSEN54::read_nox_index> noxIndex;    
};
