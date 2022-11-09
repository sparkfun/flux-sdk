/*
 *
 *  spDevMS5637.cpp
 *
 *  Spark Device object for the MS5637 device.
 *
 *
 */

#include "Arduino.h"

#include "spDevMS5637.h"

uint8_t spDevMS5637::defaultDeviceAddress[] = {MS5637_ADDR, kSparkDeviceAddressNull};

// Register this class with the system - this enables the *auto load* of this device
spRegisterDevice(spDevMS5637);

spDevMS5637::spDevMS5637()
{

    spSetupDeviceIdent(getDeviceName());
    setDescription("MS5637 Pressure and Temperature sensor");

    // Register output params
    spRegister(pressure_mbar, "Pressure (mbar)", "Atmospheric pressure in milli-bar");
    spRegister(temperatureC, "Temperature (C)", "The temperature in degrees C");
}

// Function to encapsulate the ops needed to get values from the sensor.
float spDevMS5637::read_pressure_mbar()
{
    return MS5637::getPressure();
}
float spDevMS5637::read_temperature_C()
{
    return MS5637::getTemperature();
}

// Static method used to determine if this device is connected

bool spDevMS5637::isConnected(spBusI2C &i2cDriver, uint8_t address)
{
    // For speed, ping the device address first
    if (!i2cDriver.ping(address))
        return false;

    // Read the P&T PROM memory
    uint16_t n_prom[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t *ptr = n_prom;
    for (uint8_t offset = MS5637_PROM_ADDRESS_READ_ADDRESS_0;
         offset < MS5637_PROM_ADDRESS_READ_ADDRESS_7; offset += 2)
    {
        if (!i2cDriver.readRegister16(address, offset, ptr++, false)) // Big Endian
            return false;
    }
    // Check the CRC
    uint8_t cnt, n_bit;
    uint16_t n_rem, crc_read;

    n_rem = 0x00;
    crc_read = n_prom[0];
    n_prom[MS5637_COEFFICIENT_COUNT] = 0;
    n_prom[0] = (0x0FFF & (n_prom[0])); // Clear the CRC

    for (cnt = 0; cnt < (MS5637_COEFFICIENT_COUNT + 1) * 2; cnt++)
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

    bool crcOK = (n_rem == ((n_prom[MS5637_CRC_INDEX] & 0xF000) >> 12));

    // We need to disambiguate the MS5637 (PT) from the MS8607 (PHT)

    // For speed, ping the device address first
    if (!i2cDriver.ping(0x40))
        return crcOK;

    // Let's try reading the humidity (with CRC)
    // Start a "with hold" conversion. Wait. Read the result
    uint8_t humidityWithCRC[3];
    if (!i2cDriver.readRegisterRegion(0x40, 0xE5, humidityWithCRC, 3))
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

    if (result == humidityWithCRC[2]) // If the CRC is valid we definitely have an MS8607
        return false;

    return crcOK;
}

//----------------------------------------------------------------------------------------------------------
// onInitialize()
//
// Called during the startup/initialization of the driver (after the constructor is called).
//
// Place to initialized the underlying device library/driver
//
bool spDevMS5637::onInitialize(TwoWire &wirePort)
{
    // set the underlying drivers address to the one determined during
    // device construction
    bool result = MS5637::begin(wirePort);

    if (!result)
        spLog_E("MS5637 - begin failed");

    return result;
}

