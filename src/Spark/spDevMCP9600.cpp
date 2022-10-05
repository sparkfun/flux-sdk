/*
 *
 *  spDevMCP9600.cpp
 *
 *  Driver for the MCP9600 Thermocouple Amp
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevMCP9600.h"

// Define our class static variables - allocs storage for them

// The MCP9600 supports multiple addresses (resistor-configurable)
uint8_t spDevMCP9600::defaultDeviceAddress[] = {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, kSparkDeviceAddressNull};

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
    setName(getDeviceName());
    setDescription("Microchip MCP9600 Thermocouple Amplifier.");

    // register properties
    spRegister(ambient_resolution);
    spRegister(thermocouple_resolution);
    spRegister(thermocouple_type);
    spRegister(filter_coefficent);
    spRegister(burst_samples);

    // register parameters
    spRegister(thermocouple_temp, "Thermocouple temperature", "Thermocouple temperature (C)");
    spRegister(ambient_temp, "Ambient temperature", "Ambient temperature (C)");
    spRegister(temp_delta, "Temperature delta", "Temperature change since last reset (C)");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevMCP9600::isConnected(spDevI2C &i2cDriver, uint8_t address)
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
float spDevMCP9600::read_ThermocoupleTemp(void)
{
    return MCP9600::getThermocoupleTemp();
}
float spDevMCP9600::read_AmbientTemp(void)
{
    return MCP9600::getAmbientTemp();
}
float spDevMCP9600::read_TempDelta(void)
{
    return MCP9600::getTempDelta();
}
