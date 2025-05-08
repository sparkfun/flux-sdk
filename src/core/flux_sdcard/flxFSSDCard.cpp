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

#include "flxFSSDCard.h"
#include <string>

// #include "FS.h"
#include "SD.h"

//-----------------------------------------------------------------------
// The SD systems wants filenames with full paths. The callers
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

// Global object - for quick access to FS.
_flxFSSDCard &_theSDCard = _flxFSSDCard::get();

//-----------------------------------------------------------------------
bool _flxFSSDCard::initialize()
{
    if (_isInitalized)
        return true;

    // do we have a power pin for the SD card?
    if (_pinPower)
        pinMode(_pinPower, OUTPUT);

    // pinMode(_pinCS, OUTPUT);
    // digitalWrite(_pinCS, HIGH);
    
    setPower(true);

    delay(1000);

    if (!SD.begin(_pinCS))
    {
        flxLogM_E(kMsgErrDeviceInit, "SD Card", "start");
        return false;
    }

    // if (SD_MMC.cardType() == CARD_NONE)
    // {
    //     flxLog_W(F("No SD card attached. Please insert an SD card."));
    //     return false;
    // }

    _isInitalized = true;

    // If we are here, we are ready.
    return true;
}

//-----------------------------------------------------------------------

bool _flxFSSDCard::initialize(uint8_t pinCS)
{
    if (!pinCS)
        return false;

    _pinCS = pinCS;
    return initialize();
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::initialize(uint8_t pinCS, uint8_t pinPower)
{
    if (!pinCS || !pinPower)
        return false;

    _pinCS = pinCS;
    _pinPower = pinPower;

    return initialize();
}
//-----------------------------------------------------------------------
// Power interface
void _flxFSSDCard::setPower(bool powerOn)
{
    if (!_pinPower || (_isInitalized && powerOn == _powerOn))
        return; // no need to continue

    pinMode(_pinPower, OUTPUT);
    digitalWrite(_pinPower, (powerOn ? LOW : HIGH));
    _powerOn = powerOn;
}

//-----------------------------------------------------------------------
flxFSFile _flxFSSDCard::open(const char *name, flxFileOpenMode_t mode, bool create)
{
    // Framework file
    flxFSFile theflxFile;

    if (!_isInitalized || !name || strlen(name) == 0)
        return theflxFile;

    const char *sdMode = "r";

    if (mode == kFileWrite)
        sdMode = "w+";
    else if (mode == kFileAppend)
        sdMode = "a+";

    // the MMC needs to have a full path - the filename must start with "/"

    char szBuffer[128];

    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return theflxFile;

    // File sdFile = SD.open(szBuffer, sdMode, create);
    File sdFile = SD.open(szBuffer, sdMode);

    if (sdFile)
    {

        // setup our file object that implements flxIFile interface
        flxFSSDFile theFile;

        // set the File object in our file driver interface
        theFile.setFile(sdFile);

        // Move our object to a smart pointer
        std::shared_ptr<flxIFile> pFile = std::make_shared<flxFSSDFile>(std::move(theFile));

        // Now set our driver, into the framework driver
        theflxFile.setIFile(pFile);
    }
    else
        flxLogM_E(kMsgErrFileOpen, "SD Card", name);

    return theflxFile;
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::exists(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD.exists(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::remove(const char *name)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(name, szBuffer, sizeof(szBuffer)))
        return false;

    return SD.remove(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::rename(const char *nameFrom, const char *nameTo)
{
    if (!_isInitalized)
        return false;

    char szBuffFrom[128];
    if (!checkForFullPath(nameFrom, szBuffFrom, sizeof(szBuffFrom)))
        return false;

    char szBuffTo[128];
    if (!checkForFullPath(nameTo, szBuffTo, sizeof(szBuffTo)))
        return false;

    return SD.rename(szBuffFrom, szBuffTo);
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::mkdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD.mkdir(szBuffer);
}

//-----------------------------------------------------------------------
bool _flxFSSDCard::rmdir(const char *path)
{
    if (!_isInitalized)
        return false;

    char szBuffer[128];
    if (!checkForFullPath(path, szBuffer, sizeof(szBuffer)))
        return false;

    return SD.rmdir(szBuffer);
}

//-----------------------------------------------------------------------
uint64_t _flxFSSDCard::size(void)
{
    if (!_isInitalized)
        return 0;

    return SD.size64();
}

//-----------------------------------------------------------------------
uint64_t _flxFSSDCard::total(void)
{
    if (!_isInitalized)
        return 0;

    return (uint64_t)SD.totalBlocks() * (uint64_t)SD.blockSize();
    
}
//-----------------------------------------------------------------------

const char *_flxFSSDCard::type(void)
{
    if (!_isInitalized)
        return "Unknown";

    switch (SD.type())
    {
    case SD_CARD_TYPE_SD1:
        return "SD1";
    case SD_CARD_TYPE_SD2:
        return "SD2";
    case SD_CARD_TYPE_SDHC:
        return "SDHC";
    case 0:
        return "NO CARD";

    default:
        return "UNKNOWN";
    }
}

uint64_t _flxFSSDCard::used(void)
{
    if (!_isInitalized)
        return 0;

    FSInfo fs_info;
    SDFS.info(fs_info);

    return fs_info.usedBytes;
}

// FS _flxFSSDCard::fileSystem(void)
// {
//     return FS;
// }
// -------------------------------------------------------
// File implementation

bool flxFSSDFile::isValid()
{
    // if  the file object exist, we exist
    return _file == true;
}

size_t flxFSSDFile::write(const uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.write(buf, size);
}

size_t flxFSSDFile::read(uint8_t *buf, size_t size)
{
    if (!_file)
        return false;

    return _file.read(buf, size);
}
void flxFSSDFile::close(void)
{
    if (_file)
        _file.close();
}

void flxFSSDFile::flush(void)
{
    if (_file)
        _file.flush();
}

size_t flxFSSDFile::size(void)
{
    if (_file)
        return _file.size();

    return 0;
}

const char *flxFSSDFile::name(void)
{
    if (_file)
        return _file.name();

    return nullptr;
}

bool flxFSSDFile::isDirectory(void)
{
    if (_file)
        return _file.isDirectory();

    return false;
}

std::string flxFSSDFile::getNextFilename(void)
{
    std::string tmp = "";

    if (_file)
    {
        File fNext = _file.openNextFile();
        if (fNext)
        {
            tmp = fNext.name();
            fNext.close();
        }
    }
    return tmp;
}

time_t flxFSSDFile::getLastWrite(void)
{
    if (_file)
        return _file.getLastWrite();

    return 0;
}

//-----------------------------------------------------------------------
flxFSFile flxFSSDFile::openNextFile(void)
{
    // Framework file
    flxFSFile theflxFile;

    if (!_file || !_file.isDirectory())
        return theflxFile;

    File sdFile = _file.openNextFile();

    if (sdFile)
    {

        // setup our SD file object that implements flxIFile interface
        flxFSSDFile theFile;

        // set the File object in our file driver interface
        theFile.setFile(sdFile);

        // Move our object to a smart pointer
        std::shared_ptr<flxIFile> pFile = std::make_shared<flxFSSDFile>(std::move(theFile));

        // Now set our SD driver, into the framework driver
        theflxFile.setIFile(pFile);
    }

    return theflxFile;
}

int flxFSSDFile::available(void)
{
    if (_file)
        return _file.available();

    return 0;
}

Stream *flxFSSDFile::stream(void)
{
    if (_file)
        return &_file;

    return nullptr;
}