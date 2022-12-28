

// SPI bus encapsulation

#include "flxBusSPI.h"
#include "flxCoreLog.h"
#include <Arduino.h>

flxBusSPI::flxBusSPI(void) : _spiPort{nullptr}
{
}

bool flxBusSPI::begin(bool bInit)
{
    // If the transaction settings are not provided by the user they are built here.
    SPISettings spiSettings = SPISettings(3000000, MSBFIRST, SPI_MODE3);

    // In addition of the port is not provided by the user, it defaults to SPI here.
    return begin(SPI, spiSettings, bInit);
}

bool flxBusSPI::begin(SPIClass &spiPort, SPISettings &spiSettings, bool bInit)
{

    // If we have a port, this system is init'd  - return.

    if (_spiPort)
        return true;

    _spiPort = &spiPort;

    if (bInit)
        _spiPort->begin();

    // SPI settings are needed for every transaction
    _spiSettings = spiSettings;

    return true;
}

bool flxBusSPI::writeRegisterByte(uint8_t cs, uint8_t offset, uint8_t data)
{
    // Bus?
    if (!_spiPort)
        return false;

    // Apply settings
    _spiPort->beginTransaction(_spiSettings);

    // Signal communication start
    digitalWrite(cs, LOW);

    _spiPort->transfer(offset);
    _spiPort->transfer(data);

    // End communcation
    digitalWrite(cs, HIGH);
    _spiPort->endTransaction();

    return true;
}

// Return the number of bytes sent
int flxBusSPI::writeRegisterRegion(uint8_t cs, uint8_t offset, const uint8_t *data, uint16_t length)
{

    if (!_spiPort)
        return 0;

    // Apply settings
    _spiPort->beginTransaction(_spiSettings);

    // Signal communication start
    digitalWrite(cs, LOW);
    _spiPort->transfer(offset);

    for (int i = 0; i < length; i++)
        _spiPort->transfer(*data++);

    // End communication
    digitalWrite(cs, HIGH);
    _spiPort->endTransaction();

    return length;
}

int flxBusSPI::readRegisterRegion(uint8_t cs, uint8_t reg, uint8_t *data, uint16_t length)
{
    if (!_spiPort)
        return 0;

    // Apply settings
    _spiPort->beginTransaction(_spiSettings);

    // Signal communication start
    digitalWrite(cs, LOW);

    // Note: A leading "1" must be added to transfer with register to indicate a "read"
    // Note to our future selves:
    //   This works / is required on both the ISM330 and MMC5983,
    //   but will cause badness with other SPI devices.
    //   We may need to add an alternate method if we ever add another SPI device.
    _spiPort->transfer(reg | 0x80);

    for (int i = 0; i < length; i++)
        *data++ = _spiPort->transfer(0x00);

    // End transaction
    digitalWrite(cs, HIGH);
    _spiPort->endTransaction();

    return length;
}
