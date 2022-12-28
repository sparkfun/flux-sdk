/*
 *
 *  flxDevMCP9600.cpp
 *
 *  Driver for the MCP9600 Thermocouple Amp
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevMCP9600.h"

// Define our class static variables - allocs storage for them

// The MCP9600 supports multiple addresses (resistor-configurable)
uint8_t flxDevMCP9600::defaultDeviceAddress[] = {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevMCP9600);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevMCP9600::flxDevMCP9600()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("Microchip MCP9600 Thermocouple Amplifier");

    // register properties
    spRegister(ambient_resolution, "Ambient Resolution", "Ambient temperature resolution in fractions of degrees C");
    spRegister(thermocouple_resolution, "Thermocouple Resolution", "Thermocouple temperature full-scale resolution in bits");
    spRegister(thermocouple_type, "Thermocouple Type", "The thermocouple type");
    spRegister(filter_coefficent, "Filter Coefficient", "The filter coefficient");
    spRegister(burst_samples, "Burst Samples", "The number of samples in burst mode");

    // register parameters
    spRegister(thermocouple_temp, "Thermocouple temperature", "Thermocouple temperature in degrees C");
    spRegister(ambient_temp, "Ambient temperature", "Ambient temperature in degrees C");
    spRegister(temp_delta, "Temperature delta", "Difference between the thermocouple temperature and ambient in degrees C");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevMCP9600::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{

    // The MCP9600 is a fussy device. If we call isConnected twice in succession, the second call fails
    // as the MCP9600 does not ACK on the second call. Only on the first.
    //
    // Long story short, we should not call i2cDriver.ping here. We should check the DeviceID instead.

    uint16_t devID = i2cDriver.readRegister16(address, DEVICE_ID, false); // Big Endian. This is here because the first read doesn't seem to work, but the second does. No idea why :/
    if (!i2cDriver.readRegister16(address, DEVICE_ID, &devID, false)) // Big Endian
        return false;
    return (DEV_ID_UPPER == devID >> 8);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMCP9600::onInitialize(TwoWire &wirePort)
{
    _begun = MCP9600::begin(address(), wirePort);
    return _begun;
}

void flxDevMCP9600::set_AmbientResolution(uint8_t value)
{
    _ambientResolution = value;
    if (_begun)
        MCP9600::setAmbientResolution((Ambient_Resolution)value);
}
uint8_t flxDevMCP9600::get_AmbientResolution(void)
{
    if (_begun)
        _ambientResolution = MCP9600::getAmbientResolution();
    return _ambientResolution;
}

void flxDevMCP9600::set_ThermocoupleResolution(uint8_t value)
{
    _thermoResolution = value;
    if (_begun)
        MCP9600::setThermocoupleResolution((Thermocouple_Resolution)value);
}
uint8_t flxDevMCP9600::get_ThermocoupleResolution(void)
{
    if (_begun)
        _thermoResolution = MCP9600::getThermocoupleResolution();
    return _thermoResolution;
}

void flxDevMCP9600::set_ThermocoupleType(uint8_t value)
{
    _thermoType = value;
    if (_begun)
        MCP9600::setThermocoupleType((Thermocouple_Type)value);
}
uint8_t flxDevMCP9600::get_ThermocoupleType(void)
{
    if (_begun)
        _thermoType = MCP9600::getThermocoupleType();
    return (_thermoType);
}

void flxDevMCP9600::set_FilterCoefficient(uint8_t value)
{
    _filterCoeff = value;
    if (_begun)
        MCP9600::setFilterCoefficient(value);
}
uint8_t flxDevMCP9600::get_FilterCoefficient(void)
{
    if (_begun)
        _filterCoeff = MCP9600::getFilterCoefficient();
    return _filterCoeff;
}

void flxDevMCP9600::set_BurstSamples(uint8_t value)
{
    _burstSamples = value;
    if (_begun)
        MCP9600::setBurstSamples((Burst_Sample)value);
}
uint8_t flxDevMCP9600::get_BurstSamples(void)
{
    if (_begun)
        _burstSamples = MCP9600::getBurstSamples();
    return _burstSamples;
}

// For the output param call - no args
float flxDevMCP9600::read_ThermocoupleTemp(void)
{
    return MCP9600::getThermocoupleTemp();
}
float flxDevMCP9600::read_AmbientTemp(void)
{
    return MCP9600::getAmbientTemp();
}
float flxDevMCP9600::read_TempDelta(void)
{
    return MCP9600::getTempDelta();
}
