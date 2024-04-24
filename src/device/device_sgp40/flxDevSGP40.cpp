/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

/*
 *
 *  flxDevSGP40.h
 *
 *  Spark Device object for the SGP40 device.
 *
 *
 *
 */
#include "Arduino.h"

#include "flxDevSGP40.h"

// Define our class static variables - allocs storage for them

#define kSGP40AddressDefault 0x59 // _SGP40Address is private...

uint8_t flxDevSGP40::defaultDeviceAddress[] = {kSGP40AddressDefault, kSparkDeviceAddressNull};

//----------------------------------------------------------------------------------------------------------
// Register this class with the system, enabling this driver during system
// initialization and device discovery.

flxRegisterDevice(flxDevSGP40);

//----------------------------------------------------------------------------------------------------------
// Constructor
//
// Object constructor. Performs initialization of device values, including device identifiers (name, I2C address),
// and managed properties.

flxDevSGP40::flxDevSGP40()
{

    // Setup unique identifiers for this device and basic device object systems
    setName(getDeviceName());
    setDescription("SGP40 Air Quality Sensor");

    // Register parameters
    flxRegister(vocIndex, "VOC Index", "Volatile Organic Compounds Index");

    flxRegister(rh, "Humidity (%RH)", "The relative humidity in %");
    flxRegister(temperature, "Temperature (C)", "The temperature in degrees C");
}

//----------------------------------------------------------------------------------------------------------
// Static method used to determine if devices is connected before creating this object (if creating dynamically)
bool flxDevSGP40::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    if (!i2cDriver.write(address, (uint8_t *)sgp40_measure_test, 2))
        return false;
    delay(320);

    uint8_t response[3]; // Two bytes plus CRC
    if (i2cDriver.receiveResponse(address, response, 3) != 3)
        return false;

    // Check the CRC
    uint8_t crc = 0xFF; // Init with 0xFF
    for (uint8_t x = 0; x < 2; x++)
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

    return ((crc == response[2]) && (response[0] == 0xD4) && (response[1] == 0x00));
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevSGP40::onInitialize(TwoWire &wirePort)
{

    return SGP40::begin(wirePort);
}

// GETTER methods for output params
int32_t flxDevSGP40::read_voc()
{
    return SGP40::getVOCindex(_RH, _temperature);
}

// methods for input params
void flxDevSGP40::write_rh(const float &rh)
{
    _RH = rh;
}

void flxDevSGP40::write_temperature(const float &temperature)
{
    _temperature = temperature;
}
