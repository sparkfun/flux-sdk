

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include <string>
#include "spFSSDCard.h"

#include "FS.h"
#include "SD_MMC.h"

// Global object - for quick access to Spark.
_spFSSDCard &_theFSSDCard = _spFSSDCard::get();

bool _spFSSDCard::initialize()
{
    if (_isInitalized)
        return true;

    if (!_pinCS || !_pinPower)
    {
        spLog_E("Unable to initialize SD Card device - hardware not configured.");
        return false;
    }

    pinMode(_pinPower, OUTPUT);
    pinMode(_pinCS, OUTPUT);

    digitalWrite(_pinCS, HIGH);

    setPower(true);

    delay(1000);

    if (!SD_MMC.begin())
    {
        spLog_E(F("Startup of the SD card failed."));
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();

    if (cardType == CARD_NONE)
    {
        spLog_W(F("No SD card attached. Please insert an SD card."));
        return false;
    }

    Serial.print("SD_MMC Card Type: ");
    std::string type = "UNKNOWN";
    if (cardType == CARD_MMC)
        type = "MMC";
    else if (cardType == CARD_SD)
        type = "SDSC";
    else if (cardType == CARD_SDHC)
        type = "SDHC";

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);

    spLog_I(F("SD Card type: %s, Size: %lluMB"), type.c_str(), cardSize);

    _isInitalized = true;

    // If we are here, we are ready.
    return true;
}

//-----------------------------------------------------------------------

bool _spFSSDCard::initialize(uint8_t pinCS)
{
    if (!pinCS)
        return false;

    _pinCS = pinCS;
    return initialize();
}

//-----------------------------------------------------------------------
bool _spFSSDCard::initialize(uint8_t pinCS, uint8_t pinPower)
{
    if (!pinCS || !pinPower)
        return false;

    _pinCS = pinCS;
    _pinPower = pinPower;

    return initialize();
}

// Power interface
void _spFSSDCard::setPower(bool powerOn)
{
    if (!_pinPower || (_isInitalized && powerOn == _powerOn))
        return; // no need to continue

    pinMode(_pinPower, OUTPUT);
    digitalWrite(_pinPower, (powerOn ? LOW : HIGH));
    _powerOn = powerOn;
}

spFSFile _spFSSDCard::open(const char *name, spFileOpenMode_t mode, bool create)
{

    spFSFile theSPFile;

    if (!_isInitalized)
        return theSPFile;

    const char *sdMode = FILE_READ;

    if (mode == kFileWrite)
        sdMode = FILE_WRITE;
    else if (mode == kFileAppend)
        sdMode = FILE_APPEND;

    File sdFile = SD_MMC.open(name, sdMode, create);

    if (sdFile)
        theSPFile.setFile(sdFile);

    return theSPFile;
}

bool _spFSSDCard::exists(const char *name)
{
    if (!_isInitalized)
        return false;

    return SD_MMC.exists(name);
}
bool _spFSSDCard::remove(const char *name)
{
    if (!_isInitalized)
        return false;

    return SD_MMC.remove(name);
}
bool _spFSSDCard::rename(const char *nameFrom, const char *nameTo)
{
    if (!_isInitalized)
        return false;

    return SD_MMC.rename(nameFrom, nameTo);
}

bool _spFSSDCard::mkdir(const char *path)
{
    if (!_isInitalized)
        return false;

    return SD_MMC.mkdir(path);
}

bool _spFSSDCard::rmdir(const char *path)
{
    if (!_isInitalized)
        return false;

    return SD_MMC.rmdir(path);
}

// -------------------------------------------------------
// File implementation

spFSFile::operator bool() const
{
    // if  the file object exist, we exist
    return _file == true;
}

size_t spFSFile::write(const uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.write(buf, size);
}

void spFSFile::close(void)
{
    if (_file)
        _file.close();
}
