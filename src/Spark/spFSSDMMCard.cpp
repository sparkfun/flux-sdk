

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include <string>
#include "spFSSDCard.h"

#include "FS.h"
#include "SD_MMC.h"

// Global object - for quick access to MMC FS.
_spFSSDCard &_theFSSDCard = _spFSSDCard::get();

bool _spFSSDCard::initialize()
{
    if (_isInitalized)
        return true;

    // do we have a power pin for the SD card?
    if ( _pinPower)
       pinMode(_pinPower, OUTPUT);

//    pinMode(_pinCS, OUTPUT);
//    digitalWrite(_pinCS, HIGH);

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

    std::string type = "UNKNOWN";
    if (cardType == CARD_MMC)
        type = "MMC";
    else if (cardType == CARD_SD)
        type = "SDSC";
    else if (cardType == CARD_SDHC)
        type = "SDHC";

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);

    Serial.printf("SD Card type: %s, Size: %lluMB\n\r", type.c_str(), cardSize);

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

    if (!_isInitalized || !name || strlen(name) < 4)
        return theSPFile;

    const char *sdMode = FILE_READ;

    if (mode == kFileWrite)
        sdMode = FILE_WRITE;
    else if (mode == kFileAppend)
        sdMode = FILE_APPEND;

    // the MMC needs to have a full path - the filename must start with "/"

    char szBuffer[128];
    uint8_t offset=0;
    if (*name != '/')
    {
        offset=1;
        szBuffer[0] = '/';
    }
    snprintf(szBuffer+offset, sizeof(szBuffer)-offset, "%s", name);

    File sdFile = SD_MMC.open(szBuffer, sdMode, create);

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

void spFSFile::flush(void)
{
    if (_file)
        _file.flush();
}
