/*
 *
 * QwiicDevACS37800.cpp
 *
 *  Device object for the ACS37800 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevACS37800.h"

#define kACS37800AddressDefault 0x60

// Define our class static variables - allocs storage for them

uint8_t spDevACS37800::defaultDeviceAddress[] = {kACS37800AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevACS37800);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevACS37800::spDevACS37800()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("ACS37800 Power Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register parameters
    spRegister(volts, "Voltage", "Volts (Instantaneous)");
    spRegister(amps, "Current", "Amps (Instantaneous)");
    spRegister(watts, "Power", "Watts (Instantaneous)");
    spRegister(voltsRMS, "Voltage (RMS)", "Volts (RMS)");
    spRegister(ampsRMS, "Current (RMS)", "Amps (RMS)");
    spRegister(powerActive, "Power (Active)", "Watts");
    spRegister(powerReactive, "Power (Reactive)", "VAR");
    spRegister(powerApparent, "Power (Apparent)", "VA");
    spRegister(powerFactor, "Power Factor", "Power Factor");
    spRegister(positiveAngle, "Power Angle", "True: Lagging; False: Leading");
    spRegister(positivePowerFactor, "Power Factor Sign", "True: Consumed; False: Generated");

    // Register properties
    spRegister(numberOfSamples, "Number of samples", "Number of samples");
    spRegister(bypassNenable, "Bypass n enable", "Bypass n enable");
    spRegister(senseResistance, "Sense resistance", "Sense resistance (Ohms)");
    spRegister(dividerResistance, "Divider resistance", "Divider resistance (Ohms)");
    spRegister(currentRange, "Current range", "Current range (Amps)");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevACS37800::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // It is difficult to prove if an ACS37800 is connected without (e.g.) attempting to write to volatile memory
    // So, let's prove that it is not an MCP9600 or VCNL4040

    // MCP9600
    uint8_t devID[2] = {0};
    i2cDriver.readRegisterRegion(address, 0x20, devID, 2);
    bool couldBe9600 = i2cDriver.readRegisterRegion(address, 0x20, devID, 2);
    couldBe9600 &= devID[0] == 0x40;
    if (couldBe9600)
        return false;

    // VEML4040
    uint16_t idReg; // VCNL4040_ID
    bool couldBe4040 = i2cDriver.readRegister16(address, 0x0C, &idReg, true); // Little Endian
    couldBe4040 &= (idReg == 0x0186);
    if (couldBe4040)
        return false; 

    // ACS37800
    // Read EEPROM address 0x0B and check the two ECC bits are clear. Not a great check, but something...
    uint8_t reg0B[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    bool couldBe37800 = i2cDriver.readRegisterRegion(address, 0x0B, reg0B, 4); // 32-bits, little endian
    couldBe37800 &= (reg0B[3] & 0x0A) == 0; // Check the two ECC bits are clear
    return couldBe37800;
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevACS37800::onInitialize(TwoWire &wirePort)
{

    bool result = ACS37800::begin(address(), wirePort);
    result &= ACS37800::setNumberOfSamples(1023, true) == 0; // Set the number of samples in shadow memory and eeprom
    result &= ACS37800::setBypassNenable(true, true) == 0; // Enable bypass_n in shadow memory and eeprom
    _begun = result;
    return result;
}

// GETTER methods for output params
float spDevACS37800::read_volts()
{
    if (!_volts)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _amps = true;
            _watts = true;
        }
    }
    _volts = false;
    return _theVolts;
}
float spDevACS37800::read_amps()
{
    if (!_amps)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _volts = true;
            _watts = true;
        }
    }
    _amps = false;
    return _theAmps;
}
float spDevACS37800::read_watts()
{
    if (!_watts)
    {
        if (ACS37800::readInstantaneous(&_theVolts, &_theAmps, &_theWatts) == 0)
        {
            _volts = true;
            _amps = true;
        }
    }
    _watts = false;
    return _theWatts;
}
float spDevACS37800::read_volts_rms()
{
    if (!_volts_rms)
    {
        if (ACS37800::readRMS(&_theVoltsRMS, &_theAmpsRMS) == 0)
        {
            _amps_rms = true;
        }
    }
    _volts_rms = false;
    return _theVoltsRMS;
}
float spDevACS37800::read_amps_rms()
{
    if (!_amps_rms)
    {
        if (ACS37800::readRMS(&_theVoltsRMS, &_theAmpsRMS) == 0)
        {
            _volts_rms = true;
        }
    }
    _amps_rms = false;
    return _theAmpsRMS;
}
float spDevACS37800::read_power_active()
{
    if (!_active)
    {
        if (ACS37800::readPowerActiveReactive(&_theActive, &_theReactive) == 0)
        {
            _reactive = true;
        }
    }
    _active = false;
    return _theActive;
}
float spDevACS37800::read_power_reactive()
{
    if (!_reactive)
    {
        if (ACS37800::readPowerActiveReactive(&_theActive, &_theReactive) == 0)
        {
            _active = true;
        }
    }
    _reactive = false;
    return _theReactive;
}
float spDevACS37800::read_power_apparent()
{
    if (!_apparent)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _factor = true;
            _posAngle = true;
            _posPF = true;
        }
    }
    _apparent = false;
    return _theApparent;
}
float spDevACS37800::read_power_factor()
{
    if (!_factor)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _posAngle = true;
            _posPF = true;
        }
    }
    _factor = false;
    return _theFactor;
}
bool spDevACS37800::read_pos_angle()
{
    if (!_posAngle)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _factor = true;
            _posPF = true;
        }
    }
    _posAngle = false;
    return _thePosAngle;
}
bool spDevACS37800::read_pos_power_factor()
{
    if (!_posPF)
    {
        if (ACS37800::readPowerFactor(&_theApparent, &_theFactor, &_thePosAngle, &_thePosPF) == 0)
        {
            _apparent = true;
            _factor = true;
            _posAngle = true;
        }
    }
    _posPF = false;
    return _thePosPF;
}

// methods used to get values for our RW properties

uint spDevACS37800::get_number_of_samples()
{
    uint32_t numSamples = 0;
    if (_begun)
        ACS37800::getNumberOfSamples(&numSamples);
    return numSamples;
}
void spDevACS37800::set_number_of_samples(uint numSamples)
{
    if (_begun)
        ACS37800::setNumberOfSamples(numSamples, true); // Set in EEPROM too
}
bool spDevACS37800::get_bypass_n_enable()
{
    bool bypassN = false;
    if (_begun)
        ACS37800::getBypassNenable(&bypassN);
    return bypassN;
}
void spDevACS37800::set_bypass_n_enable(bool enable)
{
    if (_begun)
        ACS37800::setBypassNenable(enable, true); // Set in EEPROM too
}
float spDevACS37800::get_sense_resistance()
{
    return _senseResistance;
}
void spDevACS37800::set_sense_resistance(float res)
{
    _senseResistance = res;
    if (_begun)
        ACS37800::setSenseRes(res);
}
float spDevACS37800::get_divider_resistance()
{
    return _dividerResistance;
}
void spDevACS37800::set_divider_resistance(float res)
{
    _dividerResistance = res;
    if (_begun)
        ACS37800::setDividerRes(res);
}
float spDevACS37800::get_current_range()
{
    return _currentRange;
}
void spDevACS37800::set_current_range(float range)
{
    _currentRange = range;
    if (_begun)
        ACS37800::setCurrentRange(range);
}

