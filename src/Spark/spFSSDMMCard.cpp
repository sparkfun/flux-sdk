

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include "spFSSDMMCard.h"
#include <string>

#include "FS.h"
#include "SD_MMC.h"

//-----------------------------------------------------------------------
// The SDMMC systems wants filenames with full paths. The callers
// the his module could miss that. Add a function to check
//
// A proper filename is returned in destbuffer.

// Returns true on success.
static bool checkForFullPath(const char *filename, char *destbuffer, size_t length)
{

    if (!filename || length < 5)
        return false;

    uint16_t offset = 0;

    if (*filename != '/')
    {
        offset = 1;
        *destbuffer = '/';
    }
    snprintf(destbuffer + offset, length - offset, "%s", filename);

    return true;
}

// Global object - for quick access to MMC FS.
_spFSSDMMCard &_theSDMMCard = _spFSSDMMCard::get();

//-----------------------------------------------------------------------
bool _spFSSDMMCard::initialize()
{
    if (_isInitalized)
        return true;

    // do we have a power pin for the SD card?
    if (_pinPower)
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

    if (SD_MMC.cardType() == CARD_NONE)
    {
        spLog_W(F("No SD card attached. Please insert an SD card."));
        return false;
    }

    _isInitalized = true;

    // If we are here, we are ready.
    return true;
}

//-----------------------------------------------------------------------

bool _spFSSDMMCard::initialize(uint8_t pinCS)
{
    if (!pinCS)
        return false;

    _pinCS = pinCS;
    return initialize();
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::initialize(uint8_t pinCS, uint8_t pinPower)
{
    if (!pinCS || !pinPower)
        return false;

    _pinCS = pinCS;
    _pinPower = pinPower;

    return initialize();
}
//-----------------------------------------------------------------------
// Power interface
void _spFSSDMMCard::setPower(bool powerOn)
{
    if (!_pinPower || (_isInitalized && powerOn == _powerOn))
        return; // no need to continue

    pinMode(_pinPower, OUTPUT);
    digitalWrite(_pinPower, (powerOn ? LOW : HIGH));
    _powerOn = powerOn;
}

//-----------------------------------------------------------------------
spFSFile _spFSSDMMCard::open(const char *name, spFileOpenMode_t mode, bool create)
{
    // Framework file
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

    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return theSPFile;

    File sdFile = SD_MMC.open(szBuffer, sdMode, create);

    if (sdFile)
    {

        // setup our MMC file object that implements spIFile interface
        spFSSDMMCFile theMMCFile;

        // set the File object in our file driver interface
        theMMCFile.setFile(sdFile);

        // Move our object to a smart pointer
        std::shared_ptr<spIFile> pMMCFile = std::make_shared<spFSSDMMCFile>(std::move(theMMCFile));

        // Now set our MMC driver, into the framework driver
        theSPFile.setIFile(pMMCFile);
    }

    return theSPFile;
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::exists(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.exists(szBuffer);
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::remove(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.remove(szBuffer);
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::rename(const char *nameFrom, const char *nameTo)
{
    if (!_isInitalized)
        return false;

    char szBuffFrom[128];
    if (!checkForFullPath(nameFrom, szBuffFrom, sizeof(szBuffFrom)))
        return false;

    char szBuffTo[128];
    if (!checkForFullPath(nameTo, szBuffTo, sizeof(szBuffTo)))
        return false;

    return SD_MMC.rename(szBuffFrom, szBuffTo);
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::mkdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.mkdir(szBuffer);
}

//-----------------------------------------------------------------------
bool _spFSSDMMCard::rmdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.rmdir(szBuffer);
}

//-----------------------------------------------------------------------
uint _spFSSDMMCard::size(void)
{
    if (!_isInitalized)
        return 0;

    return SD_MMC.cardSize() / (1024 * 1024);
}
//-----------------------------------------------------------------------

const char *_spFSSDMMCard::type(void)
{
    if (!_isInitalized)
        return nullptr;

    switch (SD_MMC.cardType())
    {
    case CARD_MMC:
        return "MMC";
    case CARD_SD:
        return "SDSC";
    case CARD_SDHC:
        return "SDHC";
    case CARD_NONE:
        return "NO CARD";

    default:
        return "UNKNOWN";
    }
}

// -------------------------------------------------------
// File implementation

bool spFSSDMMCFile::isValid()
{
    // if  the file object exist, we exist
    return _file == true;
}

size_t spFSSDMMCFile::write(const uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.write(buf, size);
}

void spFSSDMMCFile::close(void)
{
    if (_file)
        _file.close();
}

void spFSSDMMCFile::flush(void)
{
    if (_file)
        _file.flush();
}

size_t spFSSDMMCFile::size(void)
{
    if (_file)
        return _file.size();

    return 0;
}
