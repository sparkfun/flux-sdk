/*
 *---------------------------------------------------------------------------------
 *
 * Copyright (c) 2022-2023, SparkFun Electronics Inc.  All rights reserved.
 * This software includes information which is proprietary to and a
 * trade secret of SparkFun Electronics Inc.  It is not to be disclosed
 * to anyone outside of this organization. Reproduction by any means
 * whatsoever is  prohibited without express written permission.
 *
 *---------------------------------------------------------------------------------
 */

#include "flxFileRotate.h"
#include "flxClock.h"
#include "flxUtils.h"

#define kFileNameTemplate "%s%04d.txt"

// number of writes between flushes
#define kFlushIncrement 2

bool flxFileRotate::getNextFilename(std::string &strFile)
{
    // FS Set?
    if (!_theFS)
        return false;

    char szBuffer[64];
    while (true)
    {
        snprintf(szBuffer, sizeof(szBuffer), kFileNameTemplate, filePrefix.get().c_str(), _currentFileNumber());

        // - does this file already exist?
        if (!_theFS->exists(szBuffer))
            break; // free
        _currentFileNumber = _currentFileNumber() + 1;
    }
    strFile = szBuffer;

    return true;
}

//------------------------------------------------------------------------------------------------
// called to open the current log file
bool flxFileRotate::openLogFile(bool bAppend)
{
    _currentFile = _theFS->open(_currentFilename.c_str(),
                                bAppend ? flxIFileSystem::kFileAppend : flxIFileSystem::kFileWrite, true);

    if (!_currentFile)
    {
        flxLog_E(F("File Rotate - Unable to open log file %s"), _currentFilename.c_str());
        return false;
    }
    _flushCount = 0; // new file, new start

    return true;
}
//------------------------------------------------------------------------------------------------
// called when we dont' have a file open to see if an existing file meets our needs
//

bool flxFileRotate::openCurrentFile(void)
{
    // FS Set?
    if (!_theFS)
        return false;

    char szBuffer[64];

    snprintf(szBuffer, sizeof(szBuffer), kFileNameTemplate, filePrefix.get().c_str(), _currentFileNumber());

    _currentFilename = szBuffer;

    // If the file exists, no need to add a header.
    bool bExists = _theFS->exists(szBuffer);
    if (bExists)
        _headerWritten = true;

    return openLogFile(bExists); // send in true for append mode if file exists.
}
//------------------------------------------------------------------------------------------------
// Open the next log file.

bool flxFileRotate::openNextLogFile()
{

    if (_currentFile)
    {
        _currentFile.close();
        _currentFile = flxFSFile(); // "null file"
        _currentFilename = "";
    }
    // Open the next file
    std::string nextFile;
    if (!getNextFilename(nextFile))
        return false;

    _currentFilename = nextFile;

    if (!openLogFile())
        return false;

    _secsFileOpen = flxClock.epoch();

    // no header written to file yet...
    _headerWritten = false;

    // send the new file event. Will persist prop values
    on_newFile.emit();

    return true;
}

//------------------------------------------------------------------------------------------------
void flxFileRotate::write(int value)
{
    write(flx_utils::to_string(value).c_str(), true, flxLineTypeData);
}

//------------------------------------------------------------------------------------------------
void flxFileRotate::write(float value)
{
    write(flx_utils::to_string(value).c_str(), true, flxLineTypeData);
}

//------------------------------------------------------------------------------------------------
void flxFileRotate::write(const char *value, bool newline, flxLineType_t type)
{

    if (!_theFS)
        return;

    // no file - system just starting up?
    if (!_currentFile)
    {
        bool status;

        // Do we use a current file, or go for the next file?
        //
        // Reasons for Next file:
        //      - No state saved - starting new (_secsFileOpen == 0)
        //      - or if the current elapsed period has expired

        if (_secsFileOpen() == 0 || flxClock.epoch() - _secsFileOpen() > _secsRotPeriod)
            status = openNextLogFile();
        else // have state, use current file
            status = openCurrentFile();

        // error loading
        if (!status)
            return;
    }

    // Data line? write it
    if (type == flxLineTypeData)
    {
        // Write the current line out
         _currentFile.write((uint8_t *)value, strlen(value) + 1);
        // add a cr if newline set
        if (newline)
            _currentFile.write((uint8_t *)"\n", 1);
    }
    // if this is a header line, and we've not written a header, write it
    else if (type == flxLineTypeHeader && !_headerWritten)
    {
        // Write the current line out
         _currentFile.write((uint8_t *)value, strlen(value) + 1);
        // add a cr if newline set
        if (newline)
            _currentFile.write((uint8_t *)"\n", 1);

        _headerWritten = true; 
    }

    // Will we need to rotate?
    if (flxClock.epoch() - _secsFileOpen() > _secsRotPeriod)
    {
        // open the next file, send the new file event. This will cause
        // the next line out to be a "start of the file line" (i.e. header)
        // if that's how the format rolls
        if (!openNextLogFile())
            return;
    }

    // flush the file buffer?
    _flushCount = (_flushCount + 1) % kFlushIncrement;
    if (!_flushCount)
        _currentFile.flush();
}
