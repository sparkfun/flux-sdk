/*
 *
 * QwiicDevBME280.cpp
 *
 *  Device object for the BME280 Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevMCP9600.h"

#define kMCPAddressDefault 0x60

// Define our class static variables - allocs storage for them

spType spDevMCP9600::Type;
uint8_t spDevMCP9600::defaultDeviceAddress[] = {kMCPAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevMCP9600);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevMCP9600::spDevMCP9600()
{

    // Setup unique identifiers for this device and basic device object systems
    spSetupDeviceIdent(getDeviceName());

    // register properties
    spRegister(ambient_resolution);
    spRegister(thermocouple_resolution);
    spRegister(thermocouple_type);
    spRegister(filter_coefficent);
    spRegister(burst_samples);

    // register parameters
    spRegister(thermocouple_temp);
    spRegister(ambient_temp);
    spRegister(temp_delta);
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevMCP9600::isConnected(spDevI2C &i2cDriver, uint8_t address)
{

    return i2cDriver.ping(address);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMCP9600::onInitialize(TwoWire &wirePort)
{

    // set the device address
    return MCP9600::begin(address(), wirePort);
}

void spDevMCP9600::set_AmbientResolution(bool value)
{
    MCP9600::setAmbientResolution((Ambient_Resolution)value);
}

bool spDevMCP9600::get_AmbientResolution(void)
{

    return (bool)MCP9600::getAmbientResolution();
}
void spDevMCP9600::set_ThermocoupleResolution(uint value)
{

    MCP9600::setThermocoupleResolution((Thermocouple_Resolution)value);
}
uint spDevMCP9600::get_ThermocoupleResolution(void)
{

    return (uint)MCP9600::getThermocoupleResolution();
}
void spDevMCP9600::set_ThermocoupleType(uint value)
{

    MCP9600::setThermocoupleType((Thermocouple_Type)value);
}
uint spDevMCP9600::get_ThermocoupleType(void)
{

    return MCP9600::getThermocoupleType();
}
void spDevMCP9600::set_FilterCoefficient(uint value)
{

    MCP9600::setFilterCoefficient(value);
}
uint spDevMCP9600::get_FilterCoefficient(void)
{

    return MCP9600::getFilterCoefficient();
}
void spDevMCP9600::set_BurstSamples(uint value)
{

    MCP9600::setBurstSamples((Burst_Sample)value);
}
uint spDevMCP9600::get_BurstSamples(void)
{

    return MCP9600::getBurstSamples();
}
// For the output param call - no args
float spDevMCP9600::get_ThermocoupleTemp(void)
{
    return MCP9600::getThermocoupleTemp();
}
float spDevMCP9600::get_AmbientTemp(void)
{
    return MCP9600::getAmbientTemp();
}
float spDevMCP9600::get_TempDelta(void)
{
    return MCP9600::getTempDelta();
}
