/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2024, SparkFun Electronics Inc.
 *
 * SPDX-License-Identifier: MIT
 *
 *---------------------------------------------------------------------------------
 */

// Object wrapper around the SD Card object. This allows this object to
// be part of the framework.

#include "flxFSSDMMCard.h"
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
_flxFSSDMMCard &_theSDMMCard = _flxFSSDMMCard::get();

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::initialize()
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
        flxLogM_E(kMsgErrDeviceInit, "SD Card", "start");
        return false;
    }

    if (SD_MMC.cardType() == CARD_NONE)
    {
        flxLog_W(F("No SD card attached. Please insert an SD card."));
        return false;
    }

    _isInitalized = true;

    // If we are here, we are ready.
    return true;
}

//-----------------------------------------------------------------------

bool _flxFSSDMMCard::initialize(uint8_t pinCS)
{
    if (!pinCS)
        return false;

    _pinCS = pinCS;
    return initialize();
}

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::initialize(uint8_t pinCS, uint8_t pinPower)
{
    if (!pinCS || !pinPower)
        return false;

    _pinCS = pinCS;
    _pinPower = pinPower;

    return initialize();
}
//-----------------------------------------------------------------------
// Power interface
void _flxFSSDMMCard::setPower(bool powerOn)
{
    if (!_pinPower || (_isInitalized && powerOn == _powerOn))
        return; // no need to continue

    pinMode(_pinPower, OUTPUT);
    digitalWrite(_pinPower, (powerOn ? LOW : HIGH));
    _powerOn = powerOn;
}

//-----------------------------------------------------------------------
flxFSFile _flxFSSDMMCard::open(const char *name, flxFileOpenMode_t mode, bool create)
{
    // Framework file
    flxFSFile theflxFile;

    if (!_isInitalized || !name || strlen(name) == 0)
        return theflxFile;

    const char *sdMode = FILE_READ;

    if (mode == kFileWrite)
        sdMode = FILE_WRITE;
    else if (mode == kFileAppend)
        sdMode = FILE_APPEND;

    // the MMC needs to have a full path - the filename must start with "/"

    char szBuffer[128];

    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return theflxFile;

    File sdFile = SD_MMC.open(szBuffer, sdMode, create);

    if (sdFile)
    {

        // setup our MMC file object that implements flxIFile interface
        flxFSSDMMCFile theMMCFile;

        // set the File object in our file driver interface
        theMMCFile.setFile(sdFile);

        // Move our object to a smart pointer
        std::shared_ptr<flxIFile> pMMCFile = std::make_shared<flxFSSDMMCFile>(std::move(theMMCFile));

        // Now set our MMC driver, into the framework driver
        theflxFile.setIFile(pMMCFile);
    }
    else
        flxLogM_E(kMsgErrFileOpen, "SD Card", name);

    return theflxFile;
}

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::exists(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.exists(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::remove(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.remove(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::rename(const char *nameFrom, const char *nameTo)
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
bool _flxFSSDMMCard::mkdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.mkdir(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDMMCard::rmdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD_MMC.rmdir(szBuffer);
}

//-----------------------------------------------------------------------
uint64_t _flxFSSDMMCard::size(void)
{
    if (!_isInitalized)
        return 0;

    return SD_MMC.cardSize();
}

//-----------------------------------------------------------------------
uint64_t _flxFSSDMMCard::total(void)
{
    if (!_isInitalized)
        return 0;

    return SD_MMC.totalBytes();
}
//-----------------------------------------------------------------------

const char *_flxFSSDMMCard::type(void)
{
    if (!_isInitalized)
        return "Unknown";

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

uint64_t _flxFSSDMMCard::used(void)
{
    if (!_isInitalized)
        return 0;

    return SD_MMC.usedBytes();
}

FS _flxFSSDMMCard::fileSystem(void)
{
    return SD_MMC;
}
// -------------------------------------------------------
// File implementation

bool flxFSSDMMCFile::isValid()
{
    // if  the file object exist, we exist
    return _file == true;
}

size_t flxFSSDMMCFile::write(const uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.write(buf, size);
}

size_t flxFSSDMMCFile::read(uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.read(buf, size);
}
void flxFSSDMMCFile::close(void)
{
    if (_file)
        _file.close();
}

void flxFSSDMMCFile::flush(void)
{
    if (_file)
        _file.flush();
}

size_t flxFSSDMMCFile::size(void)
{
    if (_file)
        return _file.size();

    return 0;
}

const char *flxFSSDMMCFile::name(void)
{
    if (_file)
        return _file.name();

    return nullptr;
}

bool flxFSSDMMCFile::isDirectory(void)
{
    if (_file)
        return _file.isDirectory();

    return false;
}

std::string flxFSSDMMCFile::getNextFilename(void)
{
    std::string tmp = "";

    if (_file)
        tmp = _file.getNextFileName().c_str();

    return tmp;
}

time_t flxFSSDMMCFile::getLastWrite(void)
{
    if (_file)
        return _file.getLastWrite();

    return 0;
}

//-----------------------------------------------------------------------
flxFSFile flxFSSDMMCFile::openNextFile(void)
{
    // Framework file
    flxFSFile theflxFile;

    if (!_file || !_file.isDirectory())
        return theflxFile;

    File sdFile = _file.openNextFile();

    if (sdFile)
    {

        // setup our MMC file object that implements flxIFile interface
        flxFSSDMMCFile theMMCFile;

        // set the File object in our file driver interface
        theMMCFile.setFile(sdFile);

        // Move our object to a smart pointer
        std::shared_ptr<flxIFile> pMMCFile = std::make_shared<flxFSSDMMCFile>(std::move(theMMCFile));

        // Now set our MMC driver, into the framework driver
        theflxFile.setIFile(pMMCFile);
    }

    return theflxFile;
}

int flxFSSDMMCFile::available(void)
{
    if (_file)
        return _file.available();

    return 0;
}

Stream *flxFSSDMMCFile::stream(void)
{
    if (_file)
        return &_file;

    return nullptr;
}