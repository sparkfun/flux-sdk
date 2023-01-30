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
 *  flxDevSEN54.h
 *
 *  Spark Device object for the SEN54 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevSEN54.h"

#define kSEN54AddressDefault 0x69

// Define our class static variables - allocs storage for them

uint8_t flxDevSEN54::defaultDeviceAddress[] = {kSEN54AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevSEN54);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevSEN54::flxDevSEN54()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SEN54 Environmental Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    flxRegister(temperatureOffset, "Temperature Offset", "Define how warm the sensor is compared to ambient");

    // Register parameters
    flxRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    flxRegister(humidity, "Humidity (%RH)", "The relative humidity in %");
    flxRegister(massConcentrationPm1p0, "Particle Mass Concentration (1um)", "The concentration of 0.3-1.0 micron particles in ug/m^3");
    flxRegister(massConcentrationPm2p5, "Particle Mass Concentration (2.5um)", "The concentration of 0.3-2.5 micron particles in ug/m^3");
    flxRegister(massConcentrationPm4p0, "Particle Mass Concentration (4um)", "The concentration of 0.3-4.0 micron particles in ug/m^3");
    flxRegister(massConcentrationPm10p0, "Particle Mass Concentration (10um)", "The concentration of 0.3-10.0 micron particles in ug/m^3");
    flxRegister(vocIndex, "VOC Index", "The VOC Index measured in index points (1-500)");
    flxRegister(noxIndex, "NOx Index", "The NOx Index measured in index points (1-500)");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevSEN54::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // Don't ping the SEN54. It does not like it...

    uint8_t productName[9]; // Product name is 48 bytes maximum, but we'll only read the first nine here ( Three * Two bytes plus CRC)
    uint16_t productNameReg = 0xD014;
    uint8_t productNameRegBytes[2] = { (uint8_t)(productNameReg >> 8), (uint8_t)(productNameReg & 0xFF)}; // MSB first
    if (!i2cDriver.write(address, productNameRegBytes, 2))
        return false;
    delay(20);
    if (i2cDriver.receiveResponse(address, productName, 9) != 9)
        return false;

    // Check final CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 6; x < 8; x++)
    {
        crc ^= productName[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    bool couldBeSEN54 = crc == productName[8];
    couldBeSEN54 &= productName[0] == 'S';
    couldBeSEN54 &= productName[1] == 'E';
    couldBeSEN54 &= productName[3] == 'N';
    couldBeSEN54 &= productName[4] == '5';
    couldBeSEN54 &= productName[6] == '4';

    return (couldBeSEN54);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSEN54::onInitialize(TwoWire &wirePort)
{
    SensirionI2CSen5x::begin(wirePort);
    _begun = SensirionI2CSen5x::deviceReset() == 0;
    _begun &= SensirionI2CSen5x::startMeasurement() == 0;
    return _begun;
}

// GETTER methods for output params
float flxDevSEN54::read_temperature_C()
{
    if (_begun)
        if (!_temperature)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _humidity = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _temperature = false;
    return _theAmbientTemperature;
}
float flxDevSEN54::read_humidity()
{
    if (_begun)
        if (!_humidity)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _humidity = false;
    return _theAmbientHumidity;
}
float flxDevSEN54::read_voc_index()
{
    if (_begun)
        if (!_vocIndex)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _vocIndex = false;
    return _theVocIndex;
}
float flxDevSEN54::read_nox_index()
{
    if (_begun)
        if (!_noxIndex)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _vocIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _noxIndex = false;
    return _theNoxIndex;
}
float flxDevSEN54::read_mass_concentration_1p0()
{
    if (_begun)
        if (!_massConcentrationPm1p0)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _massConcentrationPm1p0 = false;
    return _theMassConcentrationPm1p0;
}
float flxDevSEN54::read_mass_concentration_2p5()
{
    if (_begun)
        if (!_massConcentrationPm2p5)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm4p0 = true;
                _massConcentrationPm10p0 = true;
            }
    _massConcentrationPm2p5 = false;
    return _theMassConcentrationPm2p5;
}
float flxDevSEN54::read_mass_concentration_4p0()
{
    if (_begun)
        if (!_massConcentrationPm4p0)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm10p0 = true;
            }
    _massConcentrationPm4p0 = false;
    return _theMassConcentrationPm4p0;
}
float flxDevSEN54::read_mass_concentration_10p0()
{
    if (_begun)
        if (!_massConcentrationPm10p0)
            if (SensirionI2CSen5x::readMeasuredValues(_theMassConcentrationPm1p0, _theMassConcentrationPm2p5,
                                                        _theMassConcentrationPm4p0, _theMassConcentrationPm10p0,
                                                        _theAmbientHumidity, _theAmbientTemperature, _theVocIndex, _theNoxIndex) == 0)
            {
                _temperature = true;
                _humidity = true;
                _vocIndex = true;
                _noxIndex = true;
                _massConcentrationPm1p0 = true;
                _massConcentrationPm2p5 = true;
                _massConcentrationPm4p0 = true;
            }
    _massConcentrationPm10p0 = false;
    return _theMassConcentrationPm10p0;
}

//----------------------------------------------------------------------------------------------------------
// RW Properties

float flxDevSEN54::get_temperature_offset()
{
    if (_begun)
        SensirionI2CSen5x::getTemperatureOffsetSimple(_theTemperatureOffset);
    return _theTemperatureOffset;
}

void flxDevSEN54::set_temperature_offset(float offset)
{
    _theTemperatureOffset = offset;
    if (_begun)
        SensirionI2CSen5x::setTemperatureOffsetSimple(offset);
}

