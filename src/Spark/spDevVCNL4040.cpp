/*
 *
 *  spDevVCNL4040.h
 *
 *  Spark Device object for the VCNL4040 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevVCNL4040.h"

// Define our class static variables - allocs storage for them

#define kVCNL4040AddressDefault 0x60 // VCNL4040_ADDR

uint8_t spDevVCNL4040::defaultDeviceAddress[] = {kVCNL4040AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevVCNL4040);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevVCNL4040::spDevVCNL4040()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VCNL4040 Distance Sensor");

    // Register parameters
    spRegister(proximity, "Proximity (mm)", "Proximity (mm)");
    spRegister(lux, "Lux", "Lux");

    // Register read-write properties
    spRegister(ledCurrent, "LED Current (mA)", "LED Current (mA) : Min 50; Max 200");
    spRegister(irDutyCycle, "IR Duty Cycle", "IR Duty Cycle : Min 40; Max 320");
    spRegister(proximityIntegrationTime, "Proximity Integration Time", "Proximity Integration Time : Min 1; Max 8");
    spRegister(proximityResolution, "Proximity Resolution", "Proximity Resolution : False : 12-bit; True : 16-bit");
    spRegister(ambientIntegrationTime, "Ambient Integration Time (ms)", "Ambient Integration Time : Min 80; Max 640");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevVCNL4040::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint16_t idReg; // VCNL4040_ID
    bool couldBe4040 = i2cDriver.readRegister16(address, 0x0C, &idReg, true); // Little Endian
    couldBe4040 &= (idReg == 0x0186);

    return (couldBe4040); 
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevVCNL4040::onInitialize(TwoWire &wirePort)
{

    bool result = VCNL4040::begin(wirePort);
    if (result)
    {
        VCNL4040::powerOnAmbient(); //Turn on ambient sensing
        VCNL4040::setLEDCurrent(_ledCurrent);
        VCNL4040::setIRDutyCycle(_irDutyCycle);
        VCNL4040::setProxIntegrationTime(_proxIntTime);
        VCNL4040::setProxResolution(_proxRes);
        VCNL4040::setAmbientIntegrationTime(_ambIntTime);
    }
    return result;
}

// GETTER methods for output params
uint spDevVCNL4040::read_proximity()
{
    return VCNL4040::getProximity();
}

uint spDevVCNL4040::read_lux()
{
    return VCNL4040::getAmbient();
}

// methods for read-write properties
uint spDevVCNL4040::get_LED_current()
{
    return _ledCurrent;
}

void spDevVCNL4040::set_LED_current(uint current)
{
    _ledCurrent = current;
    VCNL4040::setLEDCurrent(current);
}

uint spDevVCNL4040::get_IR_duty_cycle()
{
    return _irDutyCycle;
}

void spDevVCNL4040::set_IR_duty_cycle(uint duty)
{
    _irDutyCycle = duty;
    VCNL4040::setIRDutyCycle(duty);
}

uint spDevVCNL4040::get_proximity_integration_time()
{
    return  _proxIntTime;
}

void spDevVCNL4040::set_proximity_integration_time(uint intTime)
{
    _proxIntTime = intTime;
    VCNL4040::setProxIntegrationTime(intTime);
}

bool spDevVCNL4040::get_proximity_resolution()
{
    return _proxRes;
}

void spDevVCNL4040::set_proximity_resolution(bool res)
{
    _proxRes = res;
    VCNL4040::setProxResolution(res ? 16 : 12);
}

uint spDevVCNL4040::get_ambient_integration_time()
{
    return  _ambIntTime;
}

void spDevVCNL4040::set_ambient_integration_time(uint intTime)
{
    _ambIntTime = intTime;
    VCNL4040::setAmbientIntegrationTime(intTime);
}

