/*
 *
 * spDevI2C.h
 *
 * Class to encapsulate I2C operations in support of qwiic operations
 *
 * Provides methods to perform common I2C operations in support of qwiic
 * device drivers
 */

#pragma once

#include "Arduino.h"
#include <Wire.h>

class spDevI2C
{

  public:
    spDevI2C(void);

    void begin(TwoWire &wirePort = Wire);

    TwoWire *getWirePort(void)
    {
        return _i2cPort;
    }

    int receiveResponse(uint8_t i2c_address, uint8_t *outputPointer, uint8_t length);

    // ReadRegisterRegion takes a uint8 array address as input and reads
    // a chunk of memory into that array.
    bool readRegisterRegion(uint8_t i2c_address, uint8_t, uint8_t *, uint8_t);

    // readRegister reads one register
    uint8_t readRegister(uint8_t i2c_address, uint8_t offset);
    bool readRegister(uint8_t i2c_address, uint8_t offset, uint8_t *outputPointer);

    // Reads two regs and concatenates them
    // Used for two-byte reads
    uint16_t readRegister16(uint8_t i2c_address, uint8_t offset, bool littleEndian = true);
    bool readRegister16(uint8_t i2c_address, uint8_t offset, uint16_t *value, bool littleEndian = true);

    bool ping(uint8_t i2c_address); // ping the address ...

    // Writes nothing

    bool write(uint8_t i2c_address, uint8_t offset);

    // write a block of data
    bool write(uint8_t i2c_address, uint8_t *pData, uint8_t length);

    // Writes a byte;
    bool writeRegister(uint8_t i2c_address, uint8_t offset, uint8_t dataToWrite);

    bool writeRegister16(uint8_t i2c_address, uint8_t offset, uint16_t dataToWrite);

    bool writeRegister24(uint8_t i2c_address, uint8_t offset, uint32_t value);

    // write a data region
    bool writeRegisterRegion(uint8_t i2c_address, uint8_t offset, uint8_t *inputPointer, uint8_t length);

  private:
    TwoWire *_i2cPort;
};
