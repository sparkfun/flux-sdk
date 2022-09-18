

#include "spDevI2C.h"

// Constructor

spDevI2C::spDevI2C(void)
{

    _i2cPort = nullptr;
}

// Note - SoftwareWire is a subclass of TwoWire
void spDevI2C::begin(TwoWire &wirePort)
{

    wirePort.begin();
    _i2cPort = &wirePort; // Default to Wire Port. Note - hard and soft wire supported ...
}

int spDevI2C::receiveResponse(uint8_t i2c_address, uint8_t *outputPointer, uint8_t length)
{
    int nData;

    // request bytes from slave device
    _i2cPort->requestFrom(i2c_address, length);

    for (nData = 0; _i2cPort->available() && nData < length; nData++) // slave may send less than requested
        *outputPointer++ = _i2cPort->read(); // receive a byte as character, assign to buffer, inc buffer

    return nData;
}
bool spDevI2C::readRegisterRegion(uint8_t i2c_address, uint8_t offset, uint8_t *outputPointer, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    if (_i2cPort->endTransmission(false) != 0)
        return false;

    return receiveResponse(i2c_address, outputPointer, length) == length;
}

uint8_t spDevI2C::readRegister(uint8_t i2c_address, uint8_t offset)
{

    // Return value
    uint8_t result = 0;

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    _i2cPort->endTransmission();
    _i2cPort->requestFrom(i2c_address, (uint8_t)1);

    while (_i2cPort->available())  // slave may send less than requested
        result = _i2cPort->read(); // receive a byte as a proper uint8_t

    return result;
}

int16_t spDevI2C::readRegister16(uint8_t i2c_address, uint8_t offset)
{

    uint8_t myBuffer[2];
    readRegisterRegion(i2c_address, offset, myBuffer, 2); // Does memory transfer

    return (int16_t)myBuffer[0] | (int16_t)myBuffer[1] << 8;
}

//////////////////////////////////////
bool spDevI2C::ping(uint8_t i2c_address)
{

    _i2cPort->beginTransmission(i2c_address);
    return _i2cPort->endTransmission() == 0;
}

bool spDevI2C::write(uint8_t i2c_address, uint8_t offset)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    return _i2cPort->endTransmission() == 0;
}
bool spDevI2C::write(uint8_t i2c_address, uint8_t *pData, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(pData, length);
    return _i2cPort->endTransmission() == 0;
}

bool spDevI2C::writeRegister(uint8_t i2c_address, uint8_t offset, uint8_t dataToWrite)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);
    _i2cPort->write(dataToWrite);
    return _i2cPort->endTransmission() == 0;
}

bool spDevI2C::writeRegister16(uint8_t i2c_address, uint8_t offset, uint16_t dataToWrite)
{

    uint8_t buffer[2] = {(uint8_t)(dataToWrite & 0xFF), (uint8_t)(dataToWrite >> 8)};

    return writeRegisterRegion(i2c_address, offset, buffer, 3);
}

bool spDevI2C::writeRegister24(uint8_t i2c_address, uint8_t offset, uint32_t value)
{

    uint8_t buffer[3] = {(uint8_t)(value >> 16), (uint8_t)((value >> 8) & 0xFF), (uint8_t)(value & 0xFF)};

    return writeRegisterRegion(i2c_address, offset, buffer, 3);
}
bool spDevI2C::writeRegisterRegion(uint8_t i2c_address, uint8_t offset, uint8_t *inputPointer, uint8_t length)
{

    _i2cPort->beginTransmission(i2c_address);
    _i2cPort->write(offset);

    for (int i = 0; i < length; i++, inputPointer++) // send data bytes
        _i2cPort->write(*inputPointer);              // receive a byte as character

    return _i2cPort->endTransmission() == 0;
}

//
