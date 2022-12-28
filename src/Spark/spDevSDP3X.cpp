/*
 *
 *  spDevSDP3X.h
 *
 *  Spark Device object for the SDP3X device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevSDP3X.h"

// Define our class static variables - allocs storage for them

uint8_t spDevSDP3X::defaultDeviceAddress[] = {0x21, 0x22, 0x23, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevSDP3X);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevSDP3X::spDevSDP3X()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SDP3x Differential Pressure Sensor");

    // Register the properties with the system - this makes the connections needed
    // to support managed properties/public properties

    // Register Property
    spRegister(temperatureCompensation, "Temperature Compensation", "Temperature compensation mode");
    spRegister(measurementAveraging, "Measurement Averaging", "Measurement averaging");

    // Register parameters
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    spRegister(pressure, "Pressure (Pa)", "Atmospheric pressure in Pascals");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevSDP3X::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    if (!i2cDriver.write(address, (uint8_t *)SDP3x_stop_continuous_measure, 2))
        return false;
    delay(1); // Datasheet says sensor will be receptive for another command after 500us

    if (!i2cDriver.write(address, (uint8_t *)SDP3x_read_product_id_part1, 2))
        return false;
    delay(1);

    if (!i2cDriver.write(address, (uint8_t *)SDP3x_read_product_id_part2, 2))
        return false;
    delay(1);

    uint8_t serialNo[18]; // 6 * (Two bytes plus CRC)
    if (i2cDriver.receiveResponse(address, serialNo, 18) != 18)
        return false;

    // Check only the 6th CRC (there are six in total)
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 15; x < 17; x++)
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

    uint32_t prodId = ((uint32_t)serialNo[0]) << 24; // store MSB in prodId
    prodId |= ((uint32_t)serialNo[1]) << 16;
    prodId |= ((uint32_t)serialNo[3]) << 8;
    prodId |= serialNo[4]; // store LSB in prodId

    return ((crc == serialNo[17]) && ((prodId == SDP3x_product_id_SDP31) || (prodId == SDP3x_product_id_SDP32)));
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevSDP3X::onInitialize(TwoWire &wirePort)
{

    SDP3X::stopContinuousMeasurement(address(), wirePort); // Make sure continuous measurements are stopped or .begin will fail
    _begun = SDP3X::begin(address(), wirePort);
    _begun &= (SDP3X::startContinuousMeasurement((bool)_tempComp, _measAvg) == SDP3X_SUCCESS);
    return _begun;
}

// GETTER methods for output params
float spDevSDP3X::read_temperature_C()
{
    if (_temperature <= -9998)
    {
        SDP3X::readMeasurement(&_pressure, &_temperature);
    }
    float result = _temperature;
    _temperature = -9999; // Mark the temperature as stale
    return result;
}

float spDevSDP3X::read_pressure()
{
    if (_pressure <= -9998)
    {
        SDP3X::readMeasurement(&_pressure, &_temperature);
    }
    float result = _pressure;
    _pressure = -9999; // Mark the pressure as stale
    return result;
}

//----------------------------------------------------------------------------------------------------------
// RW Properties

uint8_t spDevSDP3X::get_temperature_compensation()
{
    return _tempComp;
}

void spDevSDP3X::set_temperature_compensation(uint8_t mode)
{
    _tempComp = mode;
    if (_begun)
    {
        SDP3X::stopContinuousMeasurement();
        SDP3X::startContinuousMeasurement((bool)_tempComp, _measAvg);
    }
}

uint8_t spDevSDP3X::get_measurement_averaging()
{
    return (uint8_t)_measAvg;
}

void spDevSDP3X::set_measurement_averaging(uint8_t enable)
{
    _measAvg = (bool)enable;
    if (_begun)
    {
        SDP3X::stopContinuousMeasurement();
        SDP3X::startContinuousMeasurement((bool)_tempComp, _measAvg);
    }
}
