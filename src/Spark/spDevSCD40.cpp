/*
 *
 *  spDevSCD40.h
 *
 *  Spark Device object for the SCD40 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevSCD40.h"

#define kSCD40AddressDefault SCD4x_ADDRESS

// Define our class static variables - allocs storage for them

uint8_t spDevSCD40::defaultDeviceAddress[] = {kSCD40AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevSCD40);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevSCD40::spDevSCD40()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SCD40 CO2 Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    spRegister(autoCalibrate, "Auto Calibration", "Enable / disable automatic calibration");
    spRegister(altitudeCompensation, "Altitude Compensation", "Define the sensor altitude in metres above sea level");
    spRegister(temperatureOffset, "Temperature Offset", "Define how warm the sensor is compared to ambient");

    // Register parameters
    spRegister(co2PPM, "CO2 (PPM)", "The CO2 concentration in Parts Per Million");
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    spRegister(humidity, "Humidity (%RH)", "The releative humidity in %");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevSCD40::isConnected(spDevI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // If periodic measurements are already running, getSerialNumber will fail...
    // To be safe, let's stop period measurements before we do anything else
    // Note that the sensor will only respond to other commands after waiting 500 ms after issuing
    // the stop_periodic_measurement command.
    uint8_t stopPeriodic[2] = { (uint8_t)(SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT >> 8) , (uint8_t)(SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT & 0xFF) };
    if (!i2cDriver.write(address, stopPeriodic, 2))
        return false;
    delay(500);

    uint8_t serialNo[9]; // 3 * (Two bytes plus CRC)
    uint16_t serialNoReg = SCD4x_COMMAND_GET_SERIAL_NUMBER;
    uint8_t serialNoRegBytes[2] = {(uint8_t)(serialNoReg >> 8), (uint8_t)(serialNoReg & 0xFF)}; // MSB first
    if (!i2cDriver.write(address, serialNoRegBytes, 2))
        return false;
    delay(3);
    if (!i2cDriver.receiveResponse(address, serialNo, 9))
        return false;

    // Check only the third CRC (there are three in total)
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 6; x < 8; x++)
    {
        crc ^= serialNo[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31); // x^8+x^5+x^4+1 = 0x31
            else
                crc <<= 1;
        }
    }
    return (crc == serialNo[8]);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevSCD40::onInitialize(TwoWire &wirePort)
{

    return SCD4x::begin(wirePort);
}

// GETTER methods for output params
uint spDevSCD40::read_CO2()
{
    if (_co2 == false)
    {
        SCD4x::readMeasurement();
        _temp = true;
        _rh = true;
    }
    _co2 = false;
    return SCD4x::getCO2();
}

float spDevSCD40::read_temperature_C()
{
    if (_temp == false)
    {
        SCD4x::readMeasurement();
        _co2 = true;
        _rh = true;
    }
    _temp = false;
    return SCD4x::getTemperature();
}

float spDevSCD40::read_humidity()
{
    if (_rh == false)
    {
        SCD4x::readMeasurement();
        _co2 = true;
        _temp = true;
    }
    _rh = false;
    return SCD4x::getHumidity();
}

//----------------------------------------------------------------------------------------------------------
// RW Properties

bool spDevSCD40::get_auto_calibrate()
{
    SCD4x::stopPeriodicMeasurement();
    bool enable = SCD4x::getAutomaticSelfCalibrationEnabled();
    SCD4x::startPeriodicMeasurement();
    return enable;
}

uint spDevSCD40::get_altitude_compensation()
{
    SCD4x::stopPeriodicMeasurement();
    uint altitude = SCD4x::getSensorAltitude();
    SCD4x::startPeriodicMeasurement();
    return altitude;
}

float spDevSCD40::get_temperature_offset()
{
    SCD4x::stopPeriodicMeasurement();
    float temperature = SCD4x::getTemperatureOffset();
    SCD4x::startPeriodicMeasurement();
    return temperature;
}

void spDevSCD40::set_auto_calibrate(bool enable)
{
    SCD4x::stopPeriodicMeasurement();
    SCD4x::setAutomaticSelfCalibrationEnabled(enable);
    SCD4x::startPeriodicMeasurement();
}

void spDevSCD40::set_altitude_compensation(uint compensation)
{
    SCD4x::stopPeriodicMeasurement();
    SCD4x::setSensorAltitude(compensation);
    SCD4x::startPeriodicMeasurement();
}

void spDevSCD40::set_temperature_offset(float offset)
{
    SCD4x::stopPeriodicMeasurement();
    SCD4x::setTemperatureOffset(offset);
    SCD4x::startPeriodicMeasurement();
}
