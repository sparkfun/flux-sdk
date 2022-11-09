/*
 *
 *  spDevSTC31.h
 *
 *  Spark Device object for the STC31 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevSTC31.h"

// Define our class static variables - allocs storage for them

uint8_t spDevSTC31::defaultDeviceAddress[] = {0x29, 0x2A, 0x2B, 0x2C, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevSTC31);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevSTC31::spDevSTC31()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("STC31 CO2 Sensor");

    // Register parameters
    spRegister(co2, "CO2 (%)", "The CO2 concentration in %");
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");

    spRegister(rh, "Humidity (%RH)", "Adjust the CO2 measurement using this humidity (%RH)");
    spRegister(temperatureC_In, "Temperature (C)", "Adjust the CO2 measurement using this temperature (C)");
    spRegister(pressure, "Pressure (mbar)", "Adjust the CO2 measurement using this atmospheric pressure (mbar)");

    spRegister(binaryGas, "Binary Gas", "Define the binary gas mixture and full scale concentration");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevSTC31::isConnected(spBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t readId1[2] = {(uint8_t)(STC3X_COMMAND_READ_PRODUCT_IDENTIFIER_1 >> 8), (uint8_t)(STC3X_COMMAND_READ_PRODUCT_IDENTIFIER_1 & 0xFF)};
    if (!i2cDriver.write(address, readId1, 2))
        return false;

    uint8_t readId2[2] = {(uint8_t)(STC3X_COMMAND_READ_PRODUCT_IDENTIFIER_2 >> 8), (uint8_t)(STC3X_COMMAND_READ_PRODUCT_IDENTIFIER_2 & 0xFF)};
    if (!i2cDriver.write(address, readId2, 2))
        return false;

    uint8_t response[18]; // 6 * (Two bytes plus CRC)
    if (!i2cDriver.receiveResponse(address, response, 18))
        return false;

    // Check the final CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 15; x < 17; x++)
    {
        crc ^= response[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31); // x^8+x^5+x^4+1 = 0x31
            else
                crc <<= 1;
        }
    }

    STC3x_unsigned32Bytes_t prodNo; // Storage for the product number
    prodNo.unsigned32 = 0;
    prodNo.bytes[3] = response[0];
    prodNo.bytes[2] = response[1];
    prodNo.bytes[1] = response[3];
    prodNo.bytes[0] = response[4];

    return ((crc == response[17]) && (prodNo.unsigned32 == STC3x_SENSOR_STC31));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevSTC31::onInitialize(TwoWire &wirePort)
{

    _begun = STC3x::begin(address(), wirePort);
    _begun &= STC3x::setBinaryGas((STC3X_binary_gas_type_e)_binaryGas);
    _begun &= STC3x::setRelativeHumidity(50.0);
    _begun &= STC3x::setPressure(1000);
    return _begun;
}

// GETTER methods for output params
float spDevSTC31::read_co2()
{
    if (_co2 == false)
    {
        STC3x::measureGasConcentration();
        _tempC = true;
    }
    _co2 = false;
    return (STC3x::getCO2());
}

float spDevSTC31::read_temperature_C()
{
    if (_tempC == false)
    {
        STC3x::measureGasConcentration();
        _co2 = true;
    }
    _tempC = false;
    return (STC3x::getTemperature());
}

// GETTER methods for input params
void spDevSTC31::write_rh(const float &rh)
{
    STC3x::setRelativeHumidity(rh);
}
void spDevSTC31::write_temperature(const float &temperature)
{
    STC3x::setTemperature(temperature);
}
void spDevSTC31::write_pressure(const uint &pressure)
{
    STC3x::setPressure(pressure);
}

// read-write properties
uint8_t spDevSTC31::get_binary_gas()
{
    return _binaryGas;
}
void spDevSTC31::set_binary_gas(uint8_t gas)
{
    _binaryGas = gas;
    if (_begun)
        STC3x::setBinaryGas((STC3X_binary_gas_type_e)_binaryGas);
}
