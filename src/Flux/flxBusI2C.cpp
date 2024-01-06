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

#include "flxBusI2C.h"

// Constructor

flxBusI2C::flxBusI2C(void)
{

    _i2cPort = nullptr;
}

// Note - SoftwareWire is a subclass of TwoWire
void flxBusI2C::begin(TwoWire &wirePort)
{

    wirePort.begin();
    _i2cPort = &wirePort; // Default to Wire Port. Note - hard and soft wire supported ...
}

int flxBusI2C::receiveResponse(uint8_t i2c_address, uint8_t *outputPointer, uint8_t length)
{
    int nData;

    // request bytes from slave device
    _i2cPort->requestFrom(i2c_address, length);

    for (nData = 0; _i2cPort->available() && nData < length; nData++) // slave may send less than requested
        *outputPointer++ = _i2cPort->read(); // receive a byte as character, assign to buffer, inc buffer

    return nData;
}
bool flxBusI2C::readRegisterRegion(uint8_t i2c_address, uint8_t offset, uint8_t *outputPointer, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    if (_i2cPort->endTransmission(false) != 0)
        return false;

    return receiveResponse(i2c_address, outputPointer, length) == length;
}

uint8_t flxBusI2C::readRegister(uint8_t i2c_address, uint8_t offset)
{

    // Return value
    uint8_t result = 0;

    readRegister(i2c_address, offset, &result);

    return result;
}

bool flxBusI2C::readRegister(uint8_t i2c_address, uint8_t offset, uint8_t *outputPointer)
{

    // Return value
    uint8_t result = 0;

    int nData = 0;

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    _i2cPort->endTransmission();
    _i2cPort->requestFrom(i2c_address, (uint8_t)1);

    while (_i2cPort->available()) // slave may send less than requested
    {
        result = _i2cPort->read(); // receive a byte as a proper uint8_t
        nData++;
    }

    if (nData == 1) // Only update outputPointer if a single byte was returned
        *outputPointer = result;

    return (nData == 1);
}

uint16_t flxBusI2C::readRegister16(uint8_t i2c_address, uint8_t offset, bool littleEndian)
{

    uint16_t myBuffer = 0;
    readRegister16(i2c_address, offset, &myBuffer, littleEndian); // Does memory transfer

    return myBuffer;
}

bool flxBusI2C::readRegister16(uint8_t i2c_address, uint8_t offset, uint16_t *value, bool littleEndian)
{

    uint8_t myBuffer[2];
    bool result = readRegisterRegion(i2c_address, offset, myBuffer, 2); // Does memory transfer

    if (result)
        *value = littleEndian ? (uint16_t)myBuffer[0] | (uint16_t)myBuffer[1] << 8
                              : (uint16_t)myBuffer[0] << 8 | (uint16_t)myBuffer[1];

    return result;
}

//////////////////////////////////////
bool flxBusI2C::ping(uint8_t i2c_address)
{

    _i2cPort->beginTransmission(i2c_address);
    return _i2cPort->endTransmission() == 0;
}

bool flxBusI2C::write(uint8_t i2c_address, uint8_t offset)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    return _i2cPort->endTransmission() == 0;
}
bool flxBusI2C::write(uint8_t i2c_address, uint8_t *pData, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(pData, length);
    return _i2cPort->endTransmission() == 0;
}

bool flxBusI2C::writeRegister(uint8_t i2c_address, uint8_t offset, uint8_t dataToWrite)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    _i2cPort->write(dataToWrite);
    return _i2cPort->endTransmission() == 0;
}

bool flxBusI2C::writeRegister16(uint8_t i2c_address, uint8_t offset, uint16_t dataToWrite)
{

    uint8_t buffer[2] = {(uint8_t)(dataToWrite & 0xFF), (uint8_t)(dataToWrite >> 8)};

    return writeRegisterRegion(i2c_address, offset, buffer, 3);
}

bool flxBusI2C::writeRegister24(uint8_t i2c_address, uint8_t offset, uint32_t value)
{

    uint8_t buffer[3] = {(uint8_t)(value >> 16), (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF)};

    return writeRegisterRegion(i2c_address, offset, buffer, 3);
}
bool flxBusI2C::writeRegisterRegion(uint8_t i2c_address, uint8_t offset, uint8_t *inputPointer, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);

    for (int i = 0; i < length; i++, inputPointer++) // send data bytes
        _i2cPort->write(*inputPointer);              // receive a byte as character

    return _i2cPort->endTransmission() == 0;
}

//
