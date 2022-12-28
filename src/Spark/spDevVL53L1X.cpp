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
    spRegister(distance, "Distance (mm)", "The measured distance in mm");
    spRegister(rangeStatus, "Range Status", "The measurement range status : 0 = good");
    spRegister(signalRate, "Signal Rate", "The measurement signal rate");

    // Register read-write properties
    spRegister(distanceMode, "Distance Mode", "The selected distance mode: short or long");
    spRegister(intermeasurementPeriod, "Inter-Measurement Period (ms)", "The inter-measurement period in milliseconds");
    spRegister(crosstalk, "Crosstalk", "The crosstalk compensation in counts-per-second");
    spRegister(offset, "Offset", "The measurement offset in mm");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevVL53L1X::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t modelId[2] = { (uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID >> 8) , (uint8_t)(VL53L1_IDENTIFICATION__MODEL_ID & 0xFF) };
    bool couldBeVL53L = i2cDriver.write(address, modelId, 2);
    uint8_t idReg[3];
    couldBeVL53L &= i2cDriver.receiveResponse(address, idReg, 3) == 3;
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

    _begun = (SFEVL53L1X::begin(wirePort) == 0);
    if (_begun)
    {
        (_shortDistanceMode ? SFEVL53L1X::setDistanceModeShort() : SFEVL53L1X::setDistanceModeLong());
        // Intermeasurement Period limit for short distance mode is 20:1000. For long distance mode, it is 140:1000.
        (_shortDistanceMode ? intermeasurementPeriod.setDataLimitRange(20, 1000) : intermeasurementPeriod.setDataLimitRange(140, 1000));
        uint16_t imp = SFEVL53L1X::getIntermeasurementPeriod();
        if (!_shortDistanceMode)
            if (imp < 140)
                SFEVL53L1X::setIntermeasurementPeriod(140);
        SFEVL53L1X::startRanging();
    }
    return _begun;
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
uint8_t spDevVL53L1X::get_distance_mode()
{
    if (_shortDistanceMode)
        return DISTANCE_SHORT;
    return DISTANCE_LONG;
}

void spDevVL53L1X::set_distance_mode(uint8_t mode)
{
    _shortDistanceMode = (mode == DISTANCE_SHORT);
    if (_begun)
    {
        SFEVL53L1X::stopRanging();
        (_shortDistanceMode ? SFEVL53L1X::setDistanceModeShort() : SFEVL53L1X::setDistanceModeLong());
        // Intermeasurement Period limit for short distance mode is 20:1000. For long distance mode, it is 140:1000.
        (_shortDistanceMode ? intermeasurementPeriod.setDataLimitRange(20, 1000) : intermeasurementPeriod.setDataLimitRange(140, 1000));
        uint16_t imp = SFEVL53L1X::getIntermeasurementPeriod();
        if (!_shortDistanceMode)
            if (imp < 140)
                SFEVL53L1X::setIntermeasurementPeriod(140);
        SFEVL53L1X::startRanging();
    }
}

uint16_t spDevVL53L1X::get_intermeasurment_period()
{
    if (_begun)
        _intermeasurementPeriod = SFEVL53L1X::getIntermeasurementPeriod();
    return _intermeasurementPeriod;
}

void spDevVL53L1X::set_intermeasurment_period(uint16_t period)
{
    // Validate period. This is probably redundant - given the data limit range?
    if (period < 20)
        period = 20;
    if (!_shortDistanceMode)
        if (period < 140)
            period = 140;
    if (period > 1000)
        period = 1000;
    _intermeasurementPeriod = period;

    if (_begun)
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setIntermeasurementPeriod(period);
        SFEVL53L1X::startRanging();
    }
}

uint16_t spDevVL53L1X::get_crosstalk()
{
    if (_begun)
        _crosstalk = SFEVL53L1X::getXTalk();
    return _crosstalk;
}

void spDevVL53L1X::set_crosstalk(uint16_t level)
{
    _crosstalk = level;
    if (_begun)
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setXTalk(level);
        SFEVL53L1X::startRanging();
    }
}

uint16_t spDevVL53L1X::get_offset()
{
    if (_begun)
        _offset = SFEVL53L1X::getOffset();
    return _offset;
}

void spDevVL53L1X::set_offset(uint16_t offset)
{
    _offset = offset;
    if (_begun)
    {
        SFEVL53L1X::stopRanging();
        SFEVL53L1X::setOffset(offset);
        SFEVL53L1X::startRanging();
    }
}
