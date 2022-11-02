/*
 *
 *  spDevVEML6075.h
 *
 *  Spark Device object for the VEML6075 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevVEML6075.h"

// Define our class static variables - allocs storage for them

#define kVEML6075AddressDefault VEML6075_ADDRESS

uint8_t spDevVEML6075::defaultDeviceAddress[] = {kVEML6075AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevVEML6075);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevVEML6075::spDevVEML6075()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VEML6075 UV Sensor");

    // Register parameters
    spRegister(uva, "UVA Level", "UVA Level");
    spRegister(uvb, "UVB Level", "UVB Level");
    spRegister(uvIndex, "UV Index", "UV Index");

    // Register read-write properties
    spRegister(integrationTime, "Integration Time (ms)", "Integration Time (ms)");
    spRegister(highDynamic, "Dynamic Range", "Dynamic Range");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevVEML6075::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t deviceID;
    // VEML6075::REG_ID (0x0C) is private
    if (!i2cDriver.readRegister16(address, 0x0C, &deviceID, true)) // Little Endian
        return false;

    return (deviceID == 0x0026); // VEML6075_DEVICE_ID
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevVEML6075::onInitialize(TwoWire &wirePort)
{

    _begun =  VEML6075::begin(wirePort);
    return _begun;
}

// GETTER methods for output params
float spDevVEML6075::read_uva()
{
    return VEML6075::uva();
}

float spDevVEML6075::read_uvb()
{
    return VEML6075::uvb();
}

float spDevVEML6075::read_uv_index()
{
    return VEML6075::index();
}

// methods for read-write properties
uint8_t spDevVEML6075::get_integration_time()
{
    if (_begun)
        _integrationTime = VEML6075::getIntegrationTime();
    return _integrationTime;
}

void spDevVEML6075::set_integration_time(uint8_t intTime)
{
    _integrationTime = intTime;
    if (_begun)
        VEML6075::setIntegrationTime((VEML6075::veml6075_uv_it_t)intTime);
}

uint8_t spDevVEML6075::get_high_dynamic()
{
    if (_begun)
        _highDynamic = VEML6075::getHighDynamic();
    return _highDynamic;
}

void spDevVEML6075::set_high_dynamic(uint8_t high)
{
    _highDynamic = high;
    if (_begun)
        VEML6075::setHighDynamic((VEML6075::veml6075_hd_t)high);
}
