/*
 *
 *  flxDevMS8607.cpp
 *
 *  Spark Device object for the MS8607 device.
 *
 *
 */

#include "Arduino.h"

#include "flxDevMS8607.h"

uint8_t flxDevMS8607::defaultDeviceAddress[] = {0x76, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
flxRegisterDevice(flxDevMS8607);

flxDevMS8607::flxDevMS8607()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("MS8607 Pressure, Humidity and Temperature sensor");

    // Register output params
    spRegister(pressure_mbar, "Pressure (mbar)", "Atmospheric pressure in milli-bar");
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
    spRegister(humidity, "Humidity (%RH)", "The relative humidity in %");
}

// Function to encapsulate the ops needed to get values from the sensor.
float flxDevMS8607::read_pressure_mbar()
{
    return MS8607::getPressure();
}
float flxDevMS8607::read_temperature_C()
{
    return MS8607::getTemperature();
}
float flxDevMS8607::read_humidity()
{
    return MS8607::getHumidity();
}

// Static method used to determine if this device is connected

bool flxDevMS8607::isConnected(flxBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the P&T PROM memory
    uint16_t n_prom[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t *ptr = n_prom;
    for (uint8_t offset = PROM_ADDRESS_READ_ADDRESS_0;
         offset < PROM_ADDRESS_READ_ADDRESS_7; offset += 2)
    {
        if (!i2cDriver.readRegister16(address, offset, ptr++, false)) // Big Endian
            return false;
    }
    // Check the CRC
    uint8_t cnt, n_bit;
    uint16_t n_rem, crc_read;

    n_rem = 0x00;
    crc_read = n_prom[0];
    n_prom[COEFFICIENT_NUMBERS] = 0;
    n_prom[0] = (0x0FFF & (n_prom[0])); // Clear the CRC

    for (cnt = 0; cnt < (COEFFICIENT_NUMBERS + 1) * 2; cnt++)
    {
        // Get next byte
        if (cnt % 2 == 1)
            n_rem ^= n_prom[cnt >> 1] & 0x00FF;
        else
            n_rem ^= n_prom[cnt >> 1] >> 8;

        for (n_bit = 8; n_bit > 0; n_bit--)
        {

            if (n_rem & 0x8000)
                n_rem = (n_rem << 1) ^ 0x3000;
            else
                n_rem <<= 1;
        }
    }
    n_rem >>= 12;
    n_prom[0] = crc_read; // Restore the CRC

    bool crcOK = (n_rem == ((n_prom[CRC_INDEX] & 0xF000) >> 12));

    // We need to disambiguate the MS8607 (PT) from the MS8607 (PHT)

    // For speed, ping the device address first
    if (!i2cDriver.ping(0x40))
        return crcOK;

    // Let's try reading the humidity (with CRC)
    // Start a "with hold" conversion. Wait. Read the result
    uint8_t humidityWithCRC[3];
    if (!i2cDriver.readRegisterRegion(0x40, HSENSOR_READ_HUMIDITY_W_HOLD_COMMAND, humidityWithCRC, 3))
        return crcOK;
    // Calculate the CRC
    uint32_t polynom = 0x988000; // x^8 + x^5 + x^4 + 1
    uint32_t msb = 0x800000;
    uint32_t mask = 0xFF8000;
    uint32_t result = (((uint32_t)humidityWithCRC[0]) << 16) | (((uint32_t)humidityWithCRC[1]) << 8); // Pad with zeros as specified in spec

    while (msb != 0x80)
    {

        // Check if msb of current value is 1 and apply XOR mask
        if (result & msb)
            result = ((result ^ polynom) & mask) | (result & ~mask);

        // Shift by one
        msb >>= 1;
        mask >>= 1;
        polynom >>= 1;
    }

    return (crcOK && (result == humidityWithCRC[2])); // If both CRCs are valid we definitely have an MS8607
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool flxDevMS8607::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    bool result = MS8607::begin(wirePort);

    if (!result)
        flxLog_E("MS8607 - begin failed");

    return result;
}

