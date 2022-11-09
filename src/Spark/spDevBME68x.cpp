/*
 *
 *  spDevBME68x.cpp
 *
 *  Device object for the BME68x Qwiic device.
 *
 *
 *
 */
#include "Arduino.h"

#include "spDevBME68x.h"

#define BME688_CHIP_ID_REG 0xF0 // Chip ID 0x01
#define BME680_CHIP_ID_REG 0xD0 // Chip ID 0x61

#define kBME68xAddressDefault 0x77
#define kBME68xAddressAlt1 0x76

// Define our class static variables - allocs storage for them

uint8_t spDevBME68x::defaultDeviceAddress[] = {kBME68xAddressDefault, kBME68xAddressAlt1, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

spRegisterDevice(spDevBME68x);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

spDevBME68x::spDevBME68x()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("The Bosch BME68x Atmospheric Sensor");

    // Register parameters
    spRegister(humidity, "Humidity", "The sensed humidity value");
    spRegister(temperatureC, "TemperatureC", "The sensed temperature in degrees C");
    spRegister(pressure, "Pressure", "The sensed pressure");
    spRegister(gasResistance, "Gas Resistance", "The sensed gas resistance");
    spRegister(status, "Sensor Status", "The sensor status");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool spDevBME68x::isConnected(spBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t chipID = 0;
    bool couldBe688 = i2cDriver.readRegister(address, BME688_CHIP_ID_REG, &chipID); // Should return 0x01 for BME688
    couldBe688 &= chipID == 0x01;
    bool couldBe680 = i2cDriver.readRegister(address, BME680_CHIP_ID_REG, &chipID); // Should return 0x61 for BME680
    couldBe680 &= chipID == 0x61;

    return (couldBe688 || couldBe680);
}
//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevBME68x::onInitialize(TwoWire &wirePort)
{

    Bme68x::begin(address(), wirePort); // Returns void!
    int8_t bmeStatus = Bme68x::checkStatus();
    if (bmeStatus == -1)
    {
        spLog_E("BME68X Sensor error: %s", Bme68x::statusString());
        return false;
    }
    else if (bmeStatus == 1)
    {
        spLog_W("BME68X Sensor warning: %s", Bme68x::statusString());
    }
	
	/* Set the default configuration for temperature, pressure and humidity */
	Bme68x::setTPH();

	/* Set the heater configuration to 300 deg C for 100ms for Forced mode */
	Bme68x::setHeaterProf(300, 100);

    return true;
}

// GETTER methods for output params
float spDevBME68x::read_TemperatureC()
{
    if (!_temperature)
    {
        Bme68x::setOpMode(BME68X_FORCED_MODE);
        delayMicroseconds(Bme68x::getMeasDur());

        if (Bme68x::fetchData())
        {
            Bme68x::getData(bmeData);
            _humidity = true;
            _pressure = true;
            _gasResistance = true;
            _status = true;
        }
	}
    _temperature = false;
    return bmeData.temperature;
}
float spDevBME68x::read_Humidity()
{
    if (!_humidity)
    {
        Bme68x::setOpMode(BME68X_FORCED_MODE);
        delayMicroseconds(Bme68x::getMeasDur());

        if (Bme68x::fetchData())
        {
            Bme68x::getData(bmeData);
            _temperature = true;
            _pressure = true;
            _gasResistance = true;
            _status = true;
        }
	}
    _humidity = false;
    return bmeData.humidity;
}
float spDevBME68x::read_Pressure()
{
    if (!_pressure)
    {
        Bme68x::setOpMode(BME68X_FORCED_MODE);
        delayMicroseconds(Bme68x::getMeasDur());

        if (Bme68x::fetchData())
        {
            Bme68x::getData(bmeData);
            _temperature = true;
            _humidity = true;
            _gasResistance = true;
            _status = true;
        }
	}
    _pressure = false;
    return bmeData.pressure;
}
float spDevBME68x::read_GasResistance()
{
    if (!_gasResistance)
    {
        Bme68x::setOpMode(BME68X_FORCED_MODE);
        delayMicroseconds(Bme68x::getMeasDur());

        if (Bme68x::fetchData())
        {
            Bme68x::getData(bmeData);
            _temperature = true;
            _humidity = true;
            _pressure = true;
            _status = true;
        }
	}
    _gasResistance = false;
    return bmeData.gas_resistance;
}
uint8_t spDevBME68x::read_Status()
{
    if (!_status)
    {
        Bme68x::setOpMode(BME68X_FORCED_MODE);
        delayMicroseconds(Bme68x::getMeasDur());

        if (Bme68x::fetchData())
        {
            Bme68x::getData(bmeData);
            _temperature = true;
            _humidity = true;
            _pressure = true;
            _gasResistance = true;
        }
	}
    _status = false;
    return bmeData.status;
}
