

// SPI bus encapsulation

#pragma once

#include <SPI.h>

// Define our SPI Bus class

class spBusSPI
{

  public:
    spBusSPI(void);

    SPIClass *getSPIPort(void)
    {
        return _spiPort;
    }

    bool initialized()
    {
        return _spiPort != nullptr;
    }

    bool begin(bool bInit = false);

    bool begin(SPIClass &spiPort, SPISettings &spiSettings, bool bInit = false);

    bool writeRegisterByte(uint8_t cs, uint8_t offset, uint8_t data);

    int writeRegisterRegion(uint8_t cs, uint8_t offset, const uint8_t *data, uint16_t length);

    int readRegisterRegion(uint8_t cs, uint8_t reg, uint8_t *data, uint16_t numBytes);

  private:
    SPIClass *_spiPort;

    // Settings are used for every transaction.
    SPISettings _spiSettings;
};