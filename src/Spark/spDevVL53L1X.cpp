/*
 *
 *  spDevVL53L1X.h
 *
 *  Spark Device object for the VL53L1X device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevVL53L1X.h"

// Define our class static variables - allocs storage for them

#define kVL53L1XAddressDefault 0x29 // Unshifted

uint8_t spDevVL53L1X::defaultDeviceAddress[] = {kVL53L1XAddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevVL53L1X);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevVL53L1X::spDevVL53L1X()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("VL53L1X Distance Sensor");

    // Register parameters
    spRegister(distance, "Distance (mm)", "Distance (mm)");
    spRegister(rangeStatus, "Range Status", "Range Status : 0 = Good");
    spRegister(signalRate, "Signal Rate", "Signal Rate");

    // Register read-write properties
    spRegister(distanceMode, "Distance Mode", "Distance Mode : True = Short; False = Long");
    spRegister(intermeasurementPeriod, "Inter-Measurement Period", "Inter-Measurement Period : Min 20 or 140; Max 1000");
    spRegister(crosstalk, "Crosstalk", "Crosstalk : Max 4000");
    spRegister(offset, "Offset", "Offset : Max 4000");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevVL53L1X::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t modelId[2] = { (uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID >> 8) , (uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID & 0xFF) };
    bool couldBeVL53L = i2cDriver.write(address, modelId, 2);
    uint8_t idReg[3];
    couldBeVL53L &= i2cDriver.receiveResponse(address, idReg, 3);
    uint16_t idReg16 = (((uint16_t)idReg[0]) << 8) | idReg[1];
    couldBeVL53L &= ((idReg16 == 0xEACC) || (idReg16 == 0xEBAA));

    return (couldBeVL53L);
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevVL53L1X::onInitialize(TwoWire &wirePort)
{

    bool result = (SFEVL53L1X::begin(wirePort) == 0);
    if (result)
    {
        if (_shortDistanceMode)
            SFEVL53L1X::setDistanceModeShort();
        else
            SFEVL53L1X::setDistanceModeLong();
        SFEVL53L1X::startRanging();
    }
    return result;
}

// GETTER methods for output params
uint spDevVL53L1X::read_distance()
{
    return SFEVL53L1X::getDistance();
}

uint spDevVL53L1X::read_range_status()
{
    return SFEVL53L1X::getRangeStatus();
}

uint spDevVL53L1X::read_signal_rate()
{
    return SFEVL53L1X::getSignalRate();
}

// methods for read-write properties
bool spDevVL53L1X::get_distance_mode()
{
    return _shortDistanceMode;
}

void spDevVL53L1X::set_distance_mode(bool mode)
{
    SFEVL53L1X::stopRanging();
    _shortDistanceMode = mode;
    (mode ? SFEVL53L1X::setDistanceModeShort() : SFEVL53L1X::setDistanceModeLong());
    SFEVL53L1X::startRanging();
}

uint spDevVL53L1X::get_intermeasurment_period()
{
    return SFEVL53L1X::getIntermeasurementPeriod();
}

void spDevVL53L1X::set_intermeasurment_period(uint period)
{
    SFEVL53L1X::stopRanging();
    if (period < 20)
        period = 20;
    if (!_shortDistanceMode)
        if (period < 140)
            period = 140;
    if (period > 1000)
        period = 1000;
    SFEVL53L1X::setIntermeasurementPeriod(period);
    SFEVL53L1X::startRanging();
}

uint spDevVL53L1X::get_crosstalk()
{
    return SFEVL53L1X::getXTalk();
}

void spDevVL53L1X::set_crosstalk(uint level)
{
    SFEVL53L1X::stopRanging();
    SFEVL53L1X::setXTalk(level);
    SFEVL53L1X::startRanging();
}

uint spDevVL53L1X::get_offset()
{
    return SFEVL53L1X::getOffset();
}

void spDevVL53L1X::set_offset(uint offset)
{
    SFEVL53L1X::stopRanging();
    SFEVL53L1X::setOffset(offset);
    SFEVL53L1X::startRanging();
}
