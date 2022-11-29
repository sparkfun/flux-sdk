/*
 *
 *  spDevAHT20.cpp
 *
 *  Spark Device object for the AHT20 device.
 * 
 * 
 */

#include "Arduino.h"

#include "spDevAHT20.h"

uint8_t spDevAHT20::defaultDeviceAddress[] = { AHT20_DEFAULT_ADDRESS, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevAHT20);

spDevAHT20::spDevAHT20()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("AHT20 Temperature and Humidity sensor");

    // Register output params
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    spRegister(humidity, "Humidity (%RH)", "The relative humidity in %");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevAHT20::read_temperature_c()
{
    return AHT20::getTemperature();
}
float spDevAHT20::read_humidity()
{
    return AHT20::getHumidity();
}

// Static method used to determine if this device is connected

bool spDevAHT20::isConnected(spBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    uint8_t triggerMeasurement[3] = { 0xAC, 0x33, 0x00 };
    if (!i2cDriver.write(address, triggerMeasurement, 3))
    {
        spLog_E("AHT20 isConnected triggerMeasurement failed");
        return false;
    }
    delay(80);
    uint8_t results[7];
    if (i2cDriver.receiveResponse(address, results, 7) != 7)
    {
        spLog_E("AHT20 isConnected receiveResponse failed");
        return false;
    }

    // Check CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 0; x < 6; x++)
    {
        crc ^= results[x]; // XOR-in the next input byte

        for (uint8_t i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    if (crc == results[6])
        return true;

    spLog_E("AHT20 CRC failed: 0x%02x vs 0x%02x", crc, results[6]);
    return false;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevAHT20::onInitialize(TwoWire &wirePort)
{
	// set the underlying drivers address to the one determined during
	// device construction
    bool result = AHT20::begin(wirePort);

    if (!result)
        spLog_E("AHT20 - begin failed");

    return result;
}

